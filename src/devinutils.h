/**
 * devinutils.h
 * Device independent utilities
 * 
 * Author: James Nicholson
 */

#ifndef _DEVINIT_H
#define _DEVINIT_H

int get_device(char *pathname, Device *device);

int get_available_stream(file_descriptor *fd);

#endif /** ifndef _DEVINIT.H **/