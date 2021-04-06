/**
 * myPBdriver.c
 * A driver for the K70 push buttons
 * 
 * Author: James Nicholson
 */

#include "myPBdriver.h"
#include <stdint.h>
#include <string.h>
#include "devinio.h"
#include "switchcmd.h"
#include "utils.h"
#include "devinutils.h"

Device SW1;
Device SW2;

int pbfgetc(file_descriptor fd, char *bufp, int buflen, int *charsreadp)
{
    char pbscan = switchScan() + '0';
    strncpy(&bufp[0], &pbscan, 1);
    *charsreadp = 1;
    return E_SUCCESS;
}

int pbfputc(file_descriptor *fd, char *bufp, int buflen)
{
    return E_NOT_SUPPORTED;
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
    return E_NOT_SUPPORTED;
}

int pbfopen(char *filename, file_descriptor *fd)
{
    // Get an available Stream or return an error
    int get_stream_status = get_available_stream(fd);
    if (get_stream_status != E_SUCCESS)
    {
        return get_stream_status;
    }
    return E_SUCCESS;
}

int initPB(void)
{
    /* Initialize the push buttons */
    switchcmdInit();
    // Define functions for SW1
    SW1.fgetc = pbfgetc;
    SW1.fputc = pbfputc;
    SW1.fclose = pbfclose;
    SW1.fdelete = pbfdelete;
    SW1.fcreate = pbfcreate;
    SW1.fopen = pbfopen;
    // Define functions for SW2
    SW2.fgetc = pbfgetc;
    SW2.fputc = pbfputc;
    SW2.fclose = pbfclose;
    SW2.fdelete = pbfdelete;
    SW2.fcreate = pbfcreate;
    SW2.fopen = pbfopen;
    return E_SUCCESS;
}
