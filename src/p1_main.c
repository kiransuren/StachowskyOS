//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

//this file sets up the UART
#include "uart.h"

#include "_threadsCore.h"
#include "_kernelCore.h"

// global variable for testing
int x = 1;
int y = 1;

void idleThread(void *args){
	while(1);
}

//// CASE 1: three concurrent threads
void thread1(void *args){
	while(1){
		//threadSleep(15);
		printf("s\n");	//I sleep for 50ms
		//osYield();
	}
}

void thread2(void *args){
	while(1){
		printf("y\n");		//I yield myself
		//osYield();
	}
}

void thread3(void *args){
	while(1){
		printf("f\n");	//I am forced to switch
	}
}

// CASE 2: sleep threads

//void thread4(void *args){
//	while(1){
//		threadSleep(3000);
//		printf("I sleep for 3000ms, take %d\n", x++);
//	}
//}

//void thread5(void *args){
//	while(1){
//		threadSleep(7000);
//		printf("I sleep for 7000ms, take %d\n", y++);
//	}
//}

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
	createThread(idleThread, DEFAULT_THREAD_STACK_SIZE, MAX_IDLE_PERIOD);
	
	//CASE 1: createThread(void (*func)(void *vargs), uint32_t stackSize, uint32_t priority)
	createThread(thread1, DEFAULT_THREAD_STACK_SIZE, 4);
	createThread(thread2, DEFAULT_THREAD_STACK_SIZE, 4);
	createThread(thread3, DEFAULT_THREAD_STACK_SIZE, 4);
	
	//CASE 2:
//	createThread(thread4);
//	createThread(thread5);

	
	// start kernel
	kernelStart();

	while(1);
}
