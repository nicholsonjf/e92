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

Device SW1;
Device SW2;

int pbfgetc(file_descriptor *fd)
{
    return E_SUCCESS;
}

int pbfputc(file_descriptor *fd, char *bufp, int buflen)
{
    return E_SUCCESS;
}

int pbfclose(file_descriptor *fd)
{
    return E_SUCCESS;
}

int pbfdelete(char *pathname) {
    return E_NOT_SUPPORTED;
}

int pbfcreate(char *pathname)
{
    return E_SUCCESS;
}

int pbfopen(char *filename, file_descriptor *fd)
{
    return E_SUCCESS;
}

int initPB(void)
{
    /* Initialize the push buttons */
    pushbuttonInitAll();
    // Define the struct Devices
    Device pbs[2] = {SW1, SW2};
    for (int i = 0; i < sizeof(pbs) / sizeof(pbs[0]); i++)
    {
        pbs[i].fgetc = pbfgetc;
        pbs[i].fputc = pbfputc;
        pbs[i].fclose = pbfclose;
        pbs[i].fdelete = pbfdelete;
        pbs[i].fcreate = pbfcreate;
        pbs[i].fopen = pbfopen;
    }
    return E_SUCCESS;
}