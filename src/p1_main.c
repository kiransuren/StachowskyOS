//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

//this file sets up the UART
#include "uart.h"

#include "_threadsCore.h"
#include "_kernelCore.h"

int printMutex;
int xMutex;
int ledMutex;
int x = 0;

//function to turn on (and turn off) LEDs
void turnOnLeds(unsigned int turnOnNum)
{
	//create array of GPIO pins
	int ledArr[8] = {6,5,4,3,2,31,29,28};
	//loop through 8 GPIO pins
	for(int index = 0; index < 8; index++)
	{
		//if the bit is 1, turn on LED
		if (turnOnNum & (1<<index))
		{
			if (index < 5)
			{
				LPC_GPIO2->FIOSET |= (1U<<ledArr[index]);
			}
			else
			{
				LPC_GPIO1->FIOSET |= (1U<<ledArr[index]);
			}
		}
		// if bit is 0, turn off LED
		else{
			if (index < 5)
			{
				LPC_GPIO2->FIOCLR |= (1U<<ledArr[index]);
			}
			else
			{
				LPC_GPIO1->FIOCLR |= (1U<<ledArr[index]);
			}
		}
	}
	return;
}

void idleThread(void *args){
	while(1);
}

//// CASE 1: three threads attempting to print
void thread1(void *args){
	while(1){
		osTakeMutex(printMutex, 1000);
		printf("Thread 1\n");
		osGiveMutex(printMutex);
		osYield();
	}
}
void thread2(void *args){
	while(1){
		osTakeMutex(printMutex, 1000);
		printf("Thread 2\n");
		osGiveMutex(printMutex);
		osYield();
	}
}
void thread3(void *args){
	while(1){
		osTakeMutex(printMutex, 1000);
		printf("Thread 3\n");
		osGiveMutex(printMutex);
		osYield();
	}
}

// CASE 2: 3 threads to control LEDs
//thread4 increments x
void thread4(void *args){
	while(1){
		osTakeMutex(xMutex, 1000);
		x++;
		osGiveMutex(xMutex);
	}
}
//thread5 sets the LEDs to x % 47 using LED function
void thread5(void *args){
	while(1){
		osTakeMutex(ledMutex, 1000);
		turnOnLeds(x % 47);
		osGiveMutex(ledMutex);
	}
}
//thread6 sets the LEDs to 0x71 using LED function
void thread6(void *args){
	while(1){
		osTakeMutex(ledMutex, 1000);
		turnOnLeds(0x71);
		osGiveMutex(ledMutex);
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
	
	//LED SETUP
	LPC_GPIO1->FIODIR |= 1<<28;
	LPC_GPIO1->FIODIR |= 1<<29;
	LPC_GPIO1->FIODIR |= 1<<31;
	LPC_GPIO2->FIODIR |= 1<<2;
	LPC_GPIO2->FIODIR |= 1<<3;
	LPC_GPIO2->FIODIR |= 1<<4;
	LPC_GPIO2->FIODIR |= 1<<5;
	LPC_GPIO2->FIODIR |= 1<<6;
	
	//LPC_GPIO2->FIOSET |= 1<<6;
	
	// Create threads
	createThread(idleThread, DEFAULT_THREAD_STACK_SIZE, MAX_IDLE_PERIOD, 0, false);
	
	//CASE 1: co-operative evenly time sliced
	createThread(thread1, DEFAULT_THREAD_STACK_SIZE, 1, 0, false);
	createThread(thread2, DEFAULT_THREAD_STACK_SIZE, 1, 0, false);
	createThread(thread3, DEFAULT_THREAD_STACK_SIZE, 1, 0, false);
	
	//CASE 2:
//	createThread(thread4, DEFAULT_THREAD_STACK_SIZE, 200, 0, false);
//	createThread(thread5, DEFAULT_THREAD_STACK_SIZE, 200, 0, false);
//	createThread(thread6, DEFAULT_THREAD_STACK_SIZE, 200, 0, false);



	printMutex = osCreateMutex();
	xMutex = osCreateMutex();
	ledMutex = osCreateMutex();
	
	// start kernel
	kernelStart();

	while(1);
}
