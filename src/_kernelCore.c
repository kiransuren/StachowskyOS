#include "_kernelCore.h"

thread_t threadPool[MAX_NUM_THREADS];
int osCurrentThread = 0;
int threadPoolCurrentSize = 0;

void kernelInit(void){ 
	//initializes memory structures and interrupts necessary to run the kernel
	SHPR3 |= 0xFF << 16;	//set PendSV  priority to lowest
}

void osSched(void){
	//called by the kernel to schedule which threads to run
	ICSR |= 1<<28;		// update PendSV exception bit to "pending"
	__asm("isb");			// clear pipeline
}

void osYield(void){
	
	//todo: update

	if(osCurrentThread+1 >= threadPoolCurrentSize){
		//if current back to start of thread pool for round robin
		threadPool[osCurrentThread].taskState = IDLE;
		threadPool[0].taskState = RUNNING;
		threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 16*4); //we are about to push 16 uint32_t's
		osSched();
		osCurrentThread = 0;
	}else{
		threadPool[osCurrentThread].taskState = IDLE;
		threadPool[osCurrentThread+1].taskState = RUNNING;
		threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 16*4); //we are about to push 16 uint32_t's
		osSched();
		osCurrentThread++;
	}
	
}

int taskSwitch(void){
	//set the new PSP
	__set_PSP((uint32_t)threadPool[osCurrentThread].threadStack);
	return 1; //You are free to use this return value in your
	//assembly eventually. It will be placed in r0, so be sure to
	//access it before overwriting r0
}

void kernelStart(void){
	threadPool[0].taskState = RUNNING;
	__set_CONTROL(1<<1);
	__set_PSP((uint32_t)threadPool[0].threadStack);
	osYield();
}
