#include "_threadsCore.h"

uint32_t* getMSPInitialLocation(void){
	uint32_t* x = MSP_LOCATION;
	return (uint32_t*)*x;
}

uint32_t* getNewThreadStack(uint32_t offset){
	//return getMSPInitialLocation() + offset; //adds 4 bytes to MSP per offset (based on word size)
	uint32_t tmp = ((uint32_t)(getMSPInitialLocation()) + offset);
	
	// Check if new pointer is divisible by 8
	if(!(tmp % 8 == 0)){
		tmp += 8-tmp%8;
	}
	return (uint32_t*) tmp;
}

void setThreadingWithPSP(uint32_t* threadStack){
	__set_PSP((uint32_t) threadStack);
	__set_CONTROL((uint32_t) 1<<2);
}
