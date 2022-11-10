#include "_threadsCore.h"
#include "osDefs.h"
#include "_kernelCore.h"

extern thread_t threadPool[MAX_NUM_THREADS];
extern int osCurrentThread;
extern int threadPoolCurrentSize;

uint32_t* getMSPInitialLocation(void){
	uint32_t* x = MSP_LOCATION;
	return (uint32_t*)*x;
}

uint32_t* getNewThreadStack(uint32_t offset){
	//return getMSPInitialLocation() + offset; //adds 4 bytes to MSP per offset (based on word size)
	uint32_t tmp = ((uint32_t)(getMSPInitialLocation()) - offset);
	
	// Check if new pointer is divisible by 8
	if(!(tmp % 8 == 0)){
		tmp -= tmp%8;
	}
	return (uint32_t*) tmp;
}

void setThreadingWithPSP(uint32_t* threadStack){
	__set_PSP((uint32_t) threadStack);
	__set_CONTROL((uint32_t) 1<<1);
}

int createThread(void (*func)(void *vargs), uint32_t stackSize, uint32_t priority){
	
	// check if max number of threads reached
	if(threadPoolCurrentSize >= MAX_NUM_THREADS){
			return 1;
	}

	//TODO: deal with priorities
	thread_t newThread = {getNewThreadStack(MSR_STACK_SIZE + threadPoolCurrentSize*DEFAULT_THREAD_STACK_SIZE), func, stackSize, threadPoolCurrentSize,priority,0,IDLE};
	threadPool[threadPoolCurrentSize] = newThread;
	//set status register
	*(--threadPool[threadPoolCurrentSize].threadStack)	= 1<<24;
	//set program counter function
	*(--threadPool[threadPoolCurrentSize].threadStack) = (uint32_t)func;
			
	//set "important" registers to some arbitrary values
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0xE; //LR
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0xC; //R12
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x3; //R3
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x2; //R2
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x1; //R1
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x0; // R0
			
			
	//set other registers to some arbitrary values
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0xB; //R11
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0xA; //R10
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x9; //R9
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x8; //R8
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x7; //R7
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x6; //R6
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x5; //R5
	*(--threadPool[threadPoolCurrentSize].threadStack) = 0x4; //R4
		
	threadPoolCurrentSize++;
	return 0;
}
