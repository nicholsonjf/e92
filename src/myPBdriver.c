/**
 * myPBdriver.c
 * A driver for the K70 push buttons
 * 
 * Author: James Nicholson
 */

#include <stdint.h>
#include "shell.h"
#include "devinio.h"
#include "pushbutton.h"
#include "myPBdriver.h"

int initLED(void)
{
    /* Initialize the push buttons */
    ledInitAll();
    // Define the struct Devices
    Device *pbs[] = {SW1, SW2};
    for (int i = 0; i < sizeof(pbs) / sizeof(pbs[0]); i++)
    {
        pbs[i]->fgetc = fgetc;
        pbs[i]->fputc = fputc;
        pbs[i]->fclose = fclose;
    }
    return E_SUCCESS;
}

int fgetc(void)
{
    return E_SUCCESS;
}

int fputc(void)
{
    return E_SUCCESS;
}

int fclose(void)
{
    return E_SUCCESS;
}