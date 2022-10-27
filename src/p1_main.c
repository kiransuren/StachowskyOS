//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

//this file sets up the UART
#include "uart.h"

#include "_threadsCore.h"
#include "_kernelCore.h"

// global variable for testing
int x = 0; 

void thread1(void *args){
	while(1){
		printf("Hello Monkey %d\n", x);
		//osYield();
	}
}


void thread2(void *args){
	while(1){
		printf("Oooo ooo ahh ahh!\n");
		//osYield();
	}
}

void thread3(void *args){
	while(1){
		printf("Bye Monkey! %d\n", x);
		//osYield();
	}
}

void thread4(void *args){
	while(1){
		printf("Ahhh ahhh ooo oooo\n");
		x++;
		//osYield();
	}
}

void thread5(void *args){
	while(1){
		printf("This thread should never run if max is 4!\n");
		x++;
		//osYield();
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
	createThread(thread1);
	createThread(thread2);
	createThread(thread3);
	createThread(thread4);
	createThread(thread5);
	
	// start kernel
	kernelStart();

	while(1);
}
