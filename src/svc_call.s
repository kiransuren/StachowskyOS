	AREA	handle_pend,CODE,READONLY
	EXTERN taskSwitch ;C function to be used for task switching
	EXTERN SVC_Handler_Main
	GLOBAL PendSV_Handler
	GLOBAL SVC_Handler
	PRESERVE8
PendSV_Handler
	
		MRS r0,PSP
		
		;Store the registers
		STMDB r0!,{r4-r11}
		
		;call kernel task switch
		BL taskSwitch
		
		MRS r0,PSP ;this is the new task stack
		MOV LR,#0xFFFFFFFD ;magic return value to get us back to Thread mode
		
		;LoaD Multiple Increment After, basically undo the stack pushes we did before
		LDMIA r0!,{r4-r11}
		
		;Reload PSP. Now that we've popped a bunch, PSP has to be updated
		MSR PSP,r0
		
		;return
		BX LR
		
SVC_Handler
	TST LR,#4
	ITE EQ
	MRSEQ r0, MSP
	MRSNE r0, PSP
	B SVC_Handler_Main
	END ;Now include this if this is the end of your file, otherwise don’t