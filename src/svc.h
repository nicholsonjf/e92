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

#include <stdlib.h>
#include "devinio.h"

#define SVC_MaxPriority 15
#define SVC_PriorityShift 4

// Implemented SVC numbers

#define SVC_FGETC 0
#define SVC_FPUTC 1
#define SVC_FCLOSE 2
#define SVC_FCREATE 3
#define SVC_FDELETE 4
#define SVC_FOPEN 5
#define SVC_MALLOC 6
#define SVC_FREE 7
#define SVC_DIR_LS 8

void svcInit_SetSVCPriority(unsigned char priority);
void svcHandler(void);

int SVCMyfopen(char *arg0, file_descriptor *arg1);
int SVCMyfdelete(char *arg0);
int SVCMyfcreate(char *arg0);
int SVCMyfclose(file_descriptor *arg0);
int SVCMyfgetc(file_descriptor arg0, char *arg1, int arg2, int *arg3);
int SVCMyfputc(file_descriptor *arg0, char *arg1, int arg2);
void *SVCMymalloc(uint32_t arg0);
int SVCMyfree(void *arg0);
int SVCMydir_ls(void);

#endif /* ifndef _SVC_H */
