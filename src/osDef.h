#ifndef __osDef_H 
#define __osDef_H

#include <stdint.h>

#define MAX_NUM_THREADS 8
#define DEFAULT_THREAD_STACK_SIZE 0x200
#define MSR_STACK_SIZE 0x400

typedef enum{
	BLOCKED,
	IDLE,
	RUNNING
} taskState_t;

typedef struct {
	uint32_t* threadStack;
	void (*threadFunc)(void *vargs);
	uint32_t threadStackSize;	//initalize to 512
	uint32_t taskID;
	uint32_t priority;
	taskState_t taskState;
} thread_t;

#endif
