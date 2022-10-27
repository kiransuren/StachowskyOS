#ifndef __kernelCore_H 
#define __kernelCore_H

#include <stdint.h>
#include <LPC17xx.h>
#include "osDefs.h"


extern thread_t threadPool[MAX_NUM_THREADS];
extern int osCurrentThread;
extern int threadPoolCurrentSize;

void kernelInit(void); //initializes memory structures and interrupts necessary to run the kernel

void osYield(void); //called by the thread to yield control
void osSched(void); //called by the kernel to schedule which threads to run
	
int kernelStart(void);

#endif
