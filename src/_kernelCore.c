#include "_kernelCore.h"
#include "_threadsCore.h"
#include "stdbool.h"
//This file is for printf and other IO functions
#include "stdio.h"

thread_t threadPool[MAX_NUM_THREADS];
int osCurrentThread = 0;
int threadPoolCurrentSize = 0;

void kernelInit(void){ 
	//initializes memory structures and interrupts necessary to run the kernel
	SHPR3 |= 0xFE << 16;	//set PendSV  priority to 2nd lowest
	SHPR3 |= 0xFFU << 24; //set SysTick priority to lowest
	SHPR2 |= 0xFDU << 24; //set SVC priority to strongest
}

void osYield(void){
	
	//Trigger the SVC right away.
	__ASM("SVC #0");
}

void osSched(void){
	
	// find next appropriate task to run
	int lowestThreadTimerIndex = edfSched();
	
	if(threadPool[osCurrentThread].priority < threadPool[lowestThreadTimerIndex].priority && threadPool[osCurrentThread].taskState == RUNNING){
		return;
	}
	
	//Switch current task to waiting
	if(threadPool[osCurrentThread].taskState == RUNNING){
		threadPool[osCurrentThread].taskState = WAITING;																// set "old" task to WAITING for periodicity
		threadPool[osCurrentThread].threadTimer = threadPool[osCurrentThread].priority; // set waiting period
		if(threadPool[osCurrentThread].isPeriodic){
			threadPool[osCurrentThread].threadTimer = threadPool[osCurrentThread].periodicity;
		}
	}
	
	threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 8*4); 			//we are about to push 8 uint32_t's (TAIL-CHAINED INTERRUPT)

	//deal with new task
	osCurrentThread = lowestThreadTimerIndex;
	threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
	threadPool[osCurrentThread].threadTimer = threadPool[osCurrentThread].priority; // set waiting period
			
	ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
	__asm("isb");			// clear pipeline
	
	return;
}

int edfSched(void){

	int lowestThreadTimerIndex = 0; // default thread is IDLE
	
	// dealing with periodically check of earliest deadline task
	// find next task to run
	for(int osNextThread = 0; osNextThread<threadPoolCurrentSize; osNextThread++){
		if((threadPool[osNextThread].threadTimer == 0 && threadPool[osNextThread].taskState == IDLE) && threadPool[lowestThreadTimerIndex].priority > threadPool[osNextThread].priority){
			// deadline has been reached for this thread and the priority for this thread is higher than the currently known highest priority that should be executed next
			lowestThreadTimerIndex = osNextThread;
		}
	}
	
	return lowestThreadTimerIndex;
}

int taskSwitch(void){
	//set the new PSP
	__set_PSP((uint32_t)threadPool[osCurrentThread].threadStack);
	return 1;
}

int kernelStart(void){
	if(threadPoolCurrentSize > 0){
		osCurrentThread = -1;
		setThreadingWithPSP(threadPool[0].threadStack);
		osSched();
	}
	return 1;
}

void SysTick_Handler(void){
	
	// Deal with timers
	for(int i = 0; i < threadPoolCurrentSize; i++){
		// TODO: change to switch case
			if(threadPool[i].taskState == WAITING && threadPool[i].threadTimer==0){
				// timer is complete, change task state to IDLE
				threadPool[i].taskState = IDLE;
			}else if(threadPool[i].taskState != BLOCKED && threadPool[i].threadTimer != 0){
				// timer not complete, but decerement timer
				threadPool[i].threadTimer -= 1;
			}
	}
	
	
	// Check if task has gone on for too long
	if(threadPool[osCurrentThread].threadTimer == 0 && threadPool[osCurrentThread].taskState == RUNNING){
		//Switch current task to waiting
		threadPool[osCurrentThread].taskState = WAITING;																// set "old" task to WAITING for periodicity
		threadPool[osCurrentThread].threadTimer = threadPool[osCurrentThread].priority; // set waiting period
		if(threadPool[osCurrentThread].isPeriodic){
			threadPool[osCurrentThread].threadTimer = threadPool[osCurrentThread].periodicity;
		}
		threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 8*4); 			//we are about to push 8 uint32_t's (TAIL-CHAINED INTERRUPT)
		
		// find next task to run
		int lowestThreadTimerIndex = edfSched();

		//deal with new task
		osCurrentThread = lowestThreadTimerIndex;
		threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
				
		ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
		__asm("isb");			// clear pipeline
	
	}else{
		osSched();
	}
	
	//osSched();
	
}

void threadSleep(uint32_t sleepPeriod){
	// add thread sleep in ms
	threadPool[osCurrentThread].threadTimer = sleepPeriod;
	threadPool[osCurrentThread].taskState = WAITING;
	osYield();
}

void SVC_Handler_Main(uint32_t *svc_args){
	char call = ((char*)svc_args[6])[-2];
	//Now your system call stuff looks at the value of “call” and does what
	//ever it needs to based on that information
	
	if(call == 0){
		// yield
		osSched();
	}
	// ensure idle function, right now after looping through once it just exists
}
