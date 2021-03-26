/**
 * myPBdriver.c
 * A driver for the K70 push buttons
 * 
 * Author: James Nicholson
 */

#include "myPBdriver.h"
#include <stdint.h>
#include "devinio.h"
#include "pushbutton.h"
#include "utils.h"

int pbfgetc(void)
{
    return E_SUCCESS;
}

int pbfputc(void)
{
    return E_SUCCESS;
}

int pbfclose(void)
{
    return E_SUCCESS;
}

int initPB(void)
{
    /* Initialize the push buttons */
    ledInitAll();
    // Define the struct Devices
    Device *pbs[] = {SW1, SW2};
    for (int i = 0; i < sizeof(pbs) / sizeof(pbs[0]); i++)
    {
        pbs[i]->fgetc = pbfgetc;
        pbs[i]->fputc = pbfputc;
        pbs[i]->fclose = pbfclose;
    }
    return E_SUCCESS;
}