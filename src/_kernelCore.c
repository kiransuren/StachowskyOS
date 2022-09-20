#include "_kernelCore.h"

void kernelInit(void){ 
	//initializes memory structures and interrupts necessary to run the kernel
	SHPR3 |= 0xFF << 16;	//set PendSV  priority to lowest
}
void osSched(void){
	//called by the kernel to schedule which threads to run
	ICSR |= 1<<28;		// update PendSV exception bit to "pending"
	__asm("isb");			// clear pipeline
}
