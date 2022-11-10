#ifndef __threadsCore_H 
#define __threadsCore_H

#include <stdint.h>
#include <LPC17xx.h>
#include "osDefs.h"

uint32_t* getMSPInitialLocation(void); //Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getNewThreadStack(uint32_t offset); //Returns the address of a new PSP with offset of “offset” bytes from MSP. Be careful with pointer arithmetic! It’s best to cast to an integer then back if you’re not sure.
void setThreadingWithPSP(uint32_t* threadStack); //Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register 
int createThread(void (*func)(void *vargs), uint32_t stackSize, uint32_t priority);
#endif
