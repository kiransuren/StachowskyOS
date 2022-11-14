//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

//this file sets up the UART
#include "uart.h"

#include "_threadsCore.h"
#include "_kernelCore.h"

void idleThread(void *args){
	while(1);
}

//// CASE 1: three concurrent threads with diff frequencies
void thread1(void *args){
	while(1){
		printf("s\n");
	}
}
void thread2(void *args){
	while(1){
		printf("y\n");
	}
}
void thread3(void *args){
	while(1){
		printf("f\n");
	}
}

// CASE 2: 3 threads concurrently, with yield, sleep and period
void thread4(void *args){
	while(1){
		printf("4\n");
		osYield();
	}
}
void thread5(void *args){
	while(1){
		threadSleep(50);
		printf("5\n");
	}
}
void thread6(void *args){
	while(1){
		printf("6\n");
	}
}

// CASE 3: 2 threads that both sleep for at least one second
void thread7(void *args){
	while(1){
		threadSleep(5000);
		printf("T7\n");
	}
}
void thread8(void *args){
	while(1){
		threadSleep(7000);
		printf("T8\n");
	}
}



//This is C. The expected function heading is int main(void)
int main( void ) 
{
	//Always call this function at the start. It sets up various peripherals, the clock etc. If you don't call this
	//you may see some weird behaviour
	SystemInit();
	SysTick_Config(SystemCoreClock/MASTER_CLOCK_PERIOD);

	// initialize some kernel stuff
	kernelInit();
	
	// Create threads
	createThread(idleThread, DEFAULT_THREAD_STACK_SIZE, MAX_IDLE_PERIOD, 0, false);
	
	//CASE 0: co-operative evenly time sliced
//	createThread(thread1, DEFAULT_THREAD_STACK_SIZE, 1, 0, false);
//	createThread(thread2, DEFAULT_THREAD_STACK_SIZE, 1, 0, false);
//	createThread(thread3, DEFAULT_THREAD_STACK_SIZE, 1, 0, false);
	
	//CASE 1: createThread(void (*func)(void *vargs), uint32_t stackSize, uint32_t priority)
//	createThread(thread1, DEFAULT_THREAD_STACK_SIZE, 10, 1000/256, true);
//	createThread(thread2, DEFAULT_THREAD_STACK_SIZE, 10, 1000/100, true);
//	createThread(thread3, DEFAULT_THREAD_STACK_SIZE, 10, 1000/12, true);
	
	
	//CASE 2:
//	createThread(thread4, DEFAULT_THREAD_STACK_SIZE, 10, 0, false);
//	createThread(thread5, DEFAULT_THREAD_STACK_SIZE, 10, 0, false);
//	createThread(thread6, DEFAULT_THREAD_STACK_SIZE, 10, 1000/256, true);

	//CASE 3:
	createThread(thread7, DEFAULT_THREAD_STACK_SIZE, 10, 0, false);
	createThread(thread8, DEFAULT_THREAD_STACK_SIZE, 10, 0, false);


	
	// start kernel
	kernelStart();

	while(1);
}
