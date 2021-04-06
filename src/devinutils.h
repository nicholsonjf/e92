/**
 * devinutils.h
 * Device independent utilities
 * 
 * Author: James Nicholson
 */

#ifndef _DEVINUTILS_H
#define _DEVINUTILS_H

#include "devinio.h"

#define NUMBER_OF_DEVICES 7

/**
 * Indicates whether the file system is mounted: 0 if false, 1 if true.
 */
extern int file_structure_mounted;

int get_device(char *pathname, Device **device);

int get_available_stream(file_descriptor *fd);

#endif /** ifndef _DEVINUTILS.H **/