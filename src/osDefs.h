#ifndef __osDefs_H 
#define __osDefs_H

#include <stdint.h>

#define MAX_NUM_THREADS 4
#define DEFAULT_THREAD_STACK_SIZE 0x200
#define MSR_STACK_SIZE 0x400

#define MSP_LOCATION 0x0
#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0xE000ED04

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
