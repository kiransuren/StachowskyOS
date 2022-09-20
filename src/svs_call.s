	AREA handle_pend,CODE,READONLY
	GLOBAL PendSV_Handler
	PRESERVE8
PendSV_Handler
	MOV LR,#0xFFFFFFFD ;loads LR with instruction to return form ISR, use PSP and go into Thread mode
	BX LR
	END