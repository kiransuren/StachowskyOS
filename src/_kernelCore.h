#ifndef __kernelCore_H 
#define __kernelCore_H

#include <stdint.h>
#include <LPC17xx.h>
#include "osDefs.h"

extern thread_t threadPool[MAX_NUM_THREADS];
extern mutex_t mutexPool[MAX_NUM_MUTEXES]; 
extern int osCurrentThread;
extern int threadPoolCurrentSize;

void kernelInit(void); //initializes memory structures and interrupts necessary to run the kernel

void osYield(void); //called by the thread to yield control
void osSched(void); //called by the kernel to schedule which threads to run
int edfSched(void);

int kernelStart(void);

void threadSleep(uint32_t sleepPeriod);

int osCreateMutex(void);	// creates a new mutex and adds it to the mutex pool
void osTakeMutex(uint32_t id, uint32_t waitTimeout);	// attempts to take mutex, waits timeout if not available
int osGiveMutex(uint32_t id);						// gives mutex back

int enqueue(int mutex, int thread);
int dequeue(int mutex);

#endif
