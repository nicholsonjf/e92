/**
 * myUARTdriver.c
 * A driver UART IO
 * 
 * Author: James Nicholson
 */

#include "myUARTdriver.h"
#include "my-malloc.h"
#include "utils.h"
#include <string.h>

Device UART;

#define STREAM_BUFFER_SIZE 256

int uartfgetc(file_descriptor descr, char *bufp, int buflen, int *charsreadp)
{
    char *uart_channel;
    sprintf(uart_channel, "UART%d_BASE_PTR", uartGetline((currentPCB->streams)[*fd].device_id);
    // This function will NULL terminate the string placed in bufp
    uartGetline(*uart_channel, bufp, buflen);
    return E_SUCCESS;
}

int uartfputc(file_descriptor *fd, char *bufp, int buflen)
{
    char *uart_channel;
    sprintf(uart_channel, "UART%d_BASE_PTR", uartGetline((currentPCB->streams)[*fd].device_id);
    uartPutsNL(*uart_channel, bufp);
    return E_SUCCESS;
}

int uartfclose(file_descriptor *fd)
{
    int uartfclose_status = file_close(*fd);
    if (uartfclose_status != E_SUCCESS)
    {
        return uartfclose_status;
    }
    return E_SUCCESS;
}

int uartfopen(char *pathname, file_descriptor *fd)
{
    // The number of the uart channel will always be the 9th character in a UART device path
    (currentPCB->streams)[*fd].device_id = *pathname[9];
    return E_SUCCESS;
}

int uartfcreate(char *pathname) {
    return E_NOT_SUPPORTED;
}

int uartfdelete(char *pathname)
{
    return E_NOT_SUPPORTED;
}


int inituart(void) {
    configUART();
    // Define the struct Device UART
    UART.fgetc = uartfgetc;
    UART.fputc = uartfputc;
    UART.fclose = uartfclose;
    UART.fopen = uartfopen;
    UART.fdelete = uartfdelete;
    UART.fcreate = uartfcreate;
    return E_SUCCESS;
}

void configUART(void)
{
    /* On reset (i.e., before calling mcgInit), the processor
	 * clocking starts in FEI (FLL Engaged Internal) mode.  In FEI
	 * mode and with default settings (DRST_DRS = 00, DMX32 = 0),
	 * the MCGFLLCLK, the MCGOUTCLK (MCG (Multipurpose Clock
	 * Generator) clock), and the Bus (peripheral) clock are all set
	 * to 640 * IRC.  IRC is the Internal Reference Clock which runs
	 * at 32 KHz. [See K70 Sub-Family Reference Manual, Rev. 4,
	 * Section 25.4.1.1, Table 25-22 on labeled page 670 (PDF page
	 * 677) and MCG Control 4 Register (MCG_C4) Section 25.3.4 on
	 * labeled page 655 (PDF page 662); See K70 Sub-Family Reference
	 * Manual, Rev. 2, Section 25.4.1.1, Table 25-22 on page 657 and
	 * MCG Control 4 Register (MCG_C4) Section 25.3.4 on page 641]
	 */

    /* After calling mcgInit, MCGOUTCLK is set to 120 MHz and the Bus
	 * (peripheral) clock is set to 60 MHz.*/

    /* Table 5-2 on labeled page 225 (PDF page 232) in Rev. 4
	 * (Table 5-2 on page 221 in Rev. 2) indicates that the clock
	 * used by UART0 and UART1 is the System clock (i.e., MCGOUTCLK)
	 * and that the clock used by UART2-5 is the Bus clock. */
    const int moduleClock = 60000000;
    const int KHzInHz = 1000;
    const int baud = 115200;
    uartInit(UART2_BASE_PTR, moduleClock / KHzInHz, baud);
}