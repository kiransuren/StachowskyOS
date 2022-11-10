#ifndef __osDefs_H 
#define __osDefs_H

#include <stdint.h>

#define MAX_NUM_THREADS 4
#define DEFAULT_THREAD_STACK_SIZE 0x200
#define MSR_STACK_SIZE 0x400
#define MASTER_CLOCK_PERIOD 1000 //in ms, right now it is triggered every 1ms

#define MSP_LOCATION 0x0
#define SHPR3 *(uint32_t*)0xE000ED20
#define SHPR2 *(uint32_t*)0xE000ED1C
#define ICSR *(uint32_t*)0xE000ED04

typedef enum{
	IDLE,				// ready to run 
	BLOCKED,			// TBD
	WAITING,			// waiting for threadTimer to complete
	RUNNING				// currently running
} taskState_t;

typedef struct {
	uint32_t* threadStack;				// ptr to current top of threadstack
	void (*threadFunc)(void *vargs);	// thread function ptr 
	uint32_t threadStackSize;			// initalize to DEFAULT_THREAD_STACK_SIZE
	uint32_t taskID;					// unique identifier for task
	uint32_t priority;					// priority of thread (important for scheduling)
	uint32_t threadTimer;				// contains timer variable for this thread
	taskState_t taskState;				// current state of task
} thread_t;

#endif
