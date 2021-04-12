/**
 * svc.h
 * Routines for supervisor calls
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2021, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 *
 * Copyright (c) 2021, 2014 James L. Frankel.  All rights reserved.
 */

#ifndef _SVC_H
#define _SVC_H

#include "devinio.h"

#define SVC_MaxPriority 15
#define SVC_PriorityShift 4

// Implemented SVC numbers

#define SVC_FGETC 0
#define SVC_FPUTC 1
#define SVC_FCLOSE 2
#define SVC_FCREATE 3

void svcInit_SetSVCPriority(unsigned char priority);
void svcHandler(void);

int SVCMyfcreate(char *arg0);
int SVCMyfclose(file_descriptor *arg0);
int SVCMyfgetc(file_descriptor arg0, char *arg1, int arg2, int *arg3);
int SVCMyfputc(file_descriptor *arg0, char *arg1, int arg2);

#endif /* ifndef _SVC_H */
