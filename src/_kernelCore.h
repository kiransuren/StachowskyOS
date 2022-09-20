#ifndef __kernelCore_H 
#define __kernelCore_H

#include <stdint.h>
#include <LPC17xx.h>
#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0xE000ED04

void kernelInit(void); //initializes memory structures and interrupts necessary to run the kernel
	
void osSched(void); //called by the kernel to schedule which threads to run

#endif



