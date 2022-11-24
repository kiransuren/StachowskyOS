#ifndef __osDefs_H 
#define __osDefs_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_NUM_THREADS 4
#define DEFAULT_THREAD_STACK_SIZE 0x200
#define MSR_STACK_SIZE 0x400
#define MASTER_CLOCK_PERIOD 1000 //in ms, right now it is triggered every 1ms

#define MAX_NUM_MUTEXES 10
#define MAX_THREAD_WAITING_MUTEX 10

#define MSP_LOCATION 0x0
#define SHPR3 *(uint32_t*)0xE000ED20
#define SHPR2 *(uint32_t*)0xE000ED1C
#define ICSR *(uint32_t*)0xE000ED04

#define MAX_IDLE_PERIOD 0xFFFFFE

#define FAILED -1
#define SUCCESS 1

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
	bool isPeriodic;					// if task is periodic
	uint32_t periodicity;				// periodicity if periodic
	taskState_t taskState;				// current state of task
} thread_t;

typedef struct {
	uint32_t mutexID;					// Unique ID for this mutex
	bool isFree;						// holds if mutex is currently held by a task
	uint32_t currentOwner;				// task that currently owns this mutex
	thread_t waitingQueue[MAX_THREAD_WAITING_MUTEX];
	int queueFront;
	int queueRear;
} mutex_t;

#endif
