#ifndef __threadsCore_H 
#define __threadsCore_H

#include <stdint.h>
#include <LPC17xx.h>
#include "osDef.h"

#define MSP_LOCATION 0x0
uint32_t* getMSPInitialLocation(void); //Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getNewThreadStack(uint32_t offset); //Returns the address of a new PSP with offset of “offset” bytes from MSP. Be careful with pointer arithmetic! It’s best to cast to an integer then back if you’re not sure.
void setThreadingWithPSP(uint32_t* threadStack); //Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register 
void createThread(void (*func)(void *vargs));
#endif
