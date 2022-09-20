#include "_kernelCore.h"
void kernelInit(void){ //initializes memory structures and interrupts necessary to run the kernel
	SHPR3 |= 0xFF << 16;
}
void osSched(void){ //called by the kernel to schedule which threads to run
	ICSR |= 1<<28;
	__asm("isb");
}
