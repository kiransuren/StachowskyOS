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
	
	// ROUND ROBIN - find next task to run
	int osNextThread = osCurrentThread;
	for(int i = 0; i < threadPoolCurrentSize; i++){
		osNextThread = (osNextThread+1)%(threadPoolCurrentSize);							//get next potential task to be run
		if(threadPool[osNextThread].taskState == IDLE && osNextThread >= 0){
			//printf("nex thread %d",osNextThread);
			//everything is good, continue as usual
			// deal with old task
			if(threadPool[osCurrentThread].taskState == RUNNING){
				threadPool[osCurrentThread].taskState = IDLE;															// set "old" task to IDLE
			}
			threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 8*4); //we are about to push 8 uint32_t's (TAIL-CHAINED INTERRUPT)
			
			//deal with new task
			osCurrentThread = osNextThread;
			threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
			
			ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
			__asm("isb");			// clear pipeline
			break;
		}
	}
	
	// ensure idle function, right now after looping through once it just exists
	
	return;
}

void edfSched(void){
	
	// dealing with a thread that has exceeded its max runtime and is to be forced to yield
	if(threadPool[osCurrentThread].threadTimer == 0 && threadPool[osCurrentThread].taskState == RUNNING){
		// find next task to run
		int osNextThread = osCurrentThread;
		int lowestThreadTimerIndex = osNextThread; // TODO: not good maybe
		int lowestThreadTimerValue = MAX_IDLE_PERIOD;
		for(int i = 0; i<threadPoolCurrentSize; i++){
			osNextThread = (osNextThread+1)%(threadPoolCurrentSize);							//get next potential task to be run
			if(threadPool[osNextThread].threadTimer < lowestThreadTimerValue && threadPool[osCurrentThread].taskState == IDLE){
				lowestThreadTimerIndex = osNextThread;
				lowestThreadTimerValue = threadPool[osNextThread].threadTimer;
			}
		}
		
		//Switch current task to waiting
		threadPool[osCurrentThread].taskState = WAITING;																// set "old" task to WAITING for periodicity
		threadPool[osCurrentThread].threadTimer = threadPool[osCurrentThread].priority; // set waiting period
		threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 8*4); 			//we are about to push 8 uint32_t's (TAIL-CHAINED INTERRUPT)

		//deal with new task
		osCurrentThread = lowestThreadTimerIndex;
		threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
			
		ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
		__asm("isb");			// clear pipeline
			
	}else{
		// dealing with periodically check of earliest deadline task
		// find next task to run
		int osNextThread = osCurrentThread;
		int lowestThreadTimerIndex = osNextThread; // TODO: not good maybe
		for(int i = 0; i<threadPoolCurrentSize; i++){
			osNextThread = (osNextThread+1)%(threadPoolCurrentSize);							//get next potential task to be run
			if((threadPool[osNextThread].threadTimer == 0 && threadPool[osNextThread].taskState == IDLE) && threadPool[lowestThreadTimerIndex].priority > threadPool[osNextThread].priority ){
				// deadline has been reached for this thread and the priority for this thread is higher than the currently known highest priority that should be executed next
				lowestThreadTimerIndex = osNextThread;
			}
		}
		
		if(lowestThreadTimerIndex == osCurrentThread){
			// technically do nothing, currently running thread is highest priority
			
			//TODO: refactor this to actually do nothing
			//Switch current task to waiting
			threadPool[osCurrentThread].taskState = WAITING;																// set "old" task to WAITING for periodicity
			threadPool[osCurrentThread].threadTimer = threadPool[osCurrentThread].priority; // set waiting period
			threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 8*4); 			//we are about to push 8 uint32_t's (TAIL-CHAINED INTERRUPT)

			//deal with new task
			osCurrentThread = lowestThreadTimerIndex;
			threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
				
			ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
			__asm("isb");			// clear pipeline
			
		}else{
				//Switch current task to waiting
			threadPool[osCurrentThread].taskState = WAITING;																// set "old" task to WAITING for periodicity
			threadPool[osCurrentThread].threadTimer = threadPool[osCurrentThread].priority; // set waiting period
			threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 8*4); 			//we are about to push 8 uint32_t's (TAIL-CHAINED INTERRUPT)

			//deal with new task
			osCurrentThread = lowestThreadTimerIndex;
			threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
				
			ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
			__asm("isb");			// clear pipeline
		}

	}
	
	
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
		osYield();
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
			}else if(threadPool[i].taskState != BLOCKED){
				// timer not complete, but decerement timer
				threadPool[i].threadTimer -= 1;
			}
	}
	
	edfSched();
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
		// ROUND ROBIN - find next task to run
		int osNextThread = osCurrentThread;
		for(int i = 0; i < threadPoolCurrentSize; i++){
			osNextThread = (osNextThread+1)%(threadPoolCurrentSize);							//get next potential task to be run
			if(threadPool[osNextThread].taskState == IDLE && osNextThread >= 0){
				//everything is good, continue as usual
				// deal with old task
				if(threadPool[osCurrentThread].taskState == RUNNING){
					threadPool[osCurrentThread].taskState = IDLE;															// set "old" task to IDLE
				}
				threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 8*4); //we are about to push 8 uint32_t's (TAIL-CHAINED INTERRUPT)
				
				//deal with new task
				osCurrentThread = osNextThread;
				threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
				
				ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
				__asm("isb");			// clear pipeline
				break;
			}
		}
	}
	// ensure idle function, right now after looping through once it just exists
}
