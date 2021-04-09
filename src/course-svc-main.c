/**
 * main.c
 * Supervisor call demonstration project main program
 *
 * Demonstrates using the K70 supervisor call instruction
 * 
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2021, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 *
 * Copyright (c) 2021, 2014 James L. Frankel.  All rights reserved.
 */

/*
 * Important note:
 * 
 * The file Project_Settings -> Startup_Code -> kinetis_sysinit.c needs to be modified so
 * that a pointer to the svcHandler function is in the vector table at vector 11 (0x0000002C)
 * for the SuperVisor Call (SVCall).
 * 
 * The following declaration needs to inserted earlier in the file:
 *   extern void svcHandler(void);
 *   
 * If using the GCC Toolchain, the vector table is named "InterruptVector", and the line:
 *   SVC_Handler,			(comment delimiters removed) Vector 11: SuperVisor Call (SVCall)
 * needs to be changed to:
 *   svcHandler,			(comment delimiters removed) Vector 11: SuperVisor Call (SVCall)
 *   
 * If using the Freescale Toolchain, the vector table is named "__vect_table", and the line:
 *   (tIsrFunc)UNASSIGNED_ISR,   (comment delimiters removed) 11 (0x0000002C) (prior: -)
 * needs to be changed to:
 *   (tIsrFunc)svcHandler,       (comment delimiters removed) 11 (0x0000002C) (prior: -)
 */

#include <stdio.h>
#include "svc.h"

/* Issue the SVC (Supervisor Call) instruction (See A7.7.175 on page A7-503 of the
 * ARM�v7-M Architecture Reference Manual, ARM DDI 0403Derrata 2010_Q3 (ID100710)) */
#ifdef __GNUC__
void __attribute__((naked)) pushFourBytesThenSVCEndive(void) {
	__asm("\n\
#	Force non-eight-byte alignment\n\
	push	{r6}\n\
	svc		%0\n\
#	Pop off the pushed non-eight-byte alignment word\n\
	pop		{r6}\n\
	bx		lr\n\
		" : : "I" (SVC_ENDIVE));
}
#else
__asm void pushFourBytesThenSVCEndive(void) {
//	Force non-eight-byte alignment
	push	r6
	svc		#SVC_ENDIVE
//	Pop off the pushed non-eight-byte alignment word
	pop		r6
	bx		lr
}
#endif

int main(void) {
	int i, j;
	
	printf("Starting SVCall project\n");

	/* Set the SVC handler priority */
	svcInit_SetSVCPriority(7);

	__asm("ldr r0,=73");
	/* Issue the SVC (Supervisor Call) instruction (See A7.7.175 on page A7-503 of the
	 * ARM�v7-M Architecture Reference Manual, ARM DDI 0403Derrata 2010_Q3 (ID100710)) */
	__asm("svc #4");

	SVCMyfgetc();
	
	pushFourBytesThenSVCEndive();

	SVCBroccoliRabe(17);
	
	i = 94;
	j = SVCJicama(i);
	printf("Return value is %d\n", j);
	
	i = SVCArtichoke(-100, -101, -102, -103);
	printf("Return value is %d\n", i);

	printf("Exiting SVCall project\n");
	
	return 0;
}
