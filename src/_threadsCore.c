#include "_threadsCore.h"
#include "osDef.h"
#include "_kernelCore.h"

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

void createThread(void (*func)(void *vargs)){
	//TODO: check if pool size has reached max
	thread_t newThread = {getNewThreadStack(MSR_STACK_SIZE + (threadPoolCurrentSize)*DEFAULT_THREAD_STACK_SIZE), func, DEFAULT_THREAD_STACK_SIZE, threadPoolCurrentSize,1, IDLE};
	threadPool[threadPoolCurrentSize] = newThread;
		// bootstrap the first function
	uint32_t* sp = threadPool[threadPoolCurrentSize].threadStack;
	*--sp = 1<<24;
	*--sp = (uint32_t)func;
	
	*--sp = 0xF; //LR
	*--sp = 0xE; //R12
	*--sp = 0xD; //R3
	*--sp = 0xC; //R2
	*--sp = 0xB; //R1
	*--sp = 0xA; //R0
	
	*--sp = 0x9; //R11
	*--sp = 0x8; //R10
	*--sp = 0x7; //R9
	*--sp = 0x6; //R8
	*--sp = 0x5; //R7
	*--sp = 0x4; //R6
	*--sp = 0x3; //R5
	*--sp = 0x2; //R4
	
	threadPoolCurrentSize++;
}
