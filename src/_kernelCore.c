#include "_kernelCore.h"
#include "_threadsCore.h"

thread_t threadPool[MAX_NUM_THREADS];
int osCurrentThread = 0;
int threadPoolCurrentSize = 0;

void kernelInit(void){ 
	//initializes memory structures and interrupts necessary to run the kernel
	SHPR3 |= 0xFF << 16;	//set PendSV  priority to lowest
}

void osYield(void){
	
	if(osCurrentThread >= 0)
	{
		threadPool[osCurrentThread].taskState = IDLE;															// set "old" task to IDLE
		threadPool[osCurrentThread].threadStack = (uint32_t*)(__get_PSP() - 16*4); //we are about to push 16 uint32_t's
	}

	osCurrentThread = (osCurrentThread+1)%(threadPoolCurrentSize);							//get next task to be run
	threadPool[osCurrentThread].taskState = RUNNING;														// set state of next task
	
	ICSR |= 1<<28;		// update PendSV exception bit to "pending"																																
	__asm("isb");			// clear pipeline
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
