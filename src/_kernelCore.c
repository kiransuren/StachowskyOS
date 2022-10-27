#include "_kernelCore.h"
#include "_threadsCore.h"
#include "stdbool.h"
//This file is for printf and other IO functions
#include "stdio.h"

thread_t threadPool[MAX_NUM_THREADS];
int osCurrentThread = 0;
int threadPoolCurrentSize = 0;

bool osSchedMutex = false;

void kernelInit(void){ 
	//initializes memory structures and interrupts necessary to run the kernel
	SHPR3 |= 0xFF << 16;	//set PendSV  priority to lowest
}

void osYield(void){
	
	osSchedMutex = true;
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
			threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 16*4); //we are about to push 8 uint32_t's (TAIL-CHAINED INTERRUPT)
			
			//deal with new task
			osCurrentThread = osNextThread;
			threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
			
			osSchedMutex = false;
			ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
			__asm("isb");			// clear pipeline
			break;
		}
	}
	// ensure idle function, right now after looping through once it just exists
	
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
	if(osSchedMutex) return;
	
	// Deal with timers
	for(int i = 0; i < threadPoolCurrentSize; i++){
		// TODO: change to switch case
			if(threadPool[i].taskState == WAITING && threadPool[i].threadTimer==0){
				// timer is complete, change task state to IDLE
				threadPool[i].taskState = IDLE;
			}else if(threadPool[i].taskState == WAITING){
				// timer not complete, but decerement timer
				threadPool[i].threadTimer -= 1;
			}
	}
	
	osSched();
}

void threadSleep(uint32_t sleepPeriod){
	// add thread sleep in ms
	osSchedMutex = true;
	threadPool[osCurrentThread].threadTimer = sleepPeriod;
	threadPool[osCurrentThread].taskState = WAITING;
	osSchedMutex = false;
	osYield();
}
