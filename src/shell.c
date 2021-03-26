#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "my-malloc.h"
#include "shell.h"
#include "uart.h"
#include "delay.h"
#include "uartNL.h"
#include "derivative.h"
#include "devinio.h"
#include "myFAT32driver.h"
#include "SDHC_FAT32_Files.h"
#include "breakpoint.h"
#include "unit_tests.h"


char *help_text =
"\n"
"**********************\n"
"*** JELL HELP TEXT ***\n"
"**********************\n"
"\n"
"Help text format:\n"
"~[command]~\n"
"Help text for [command]\n"
"\n"
"~exit~\n"
"Exit from the shell(i.e., cause the shell to terminate).\n"
"\n"
"~echo~\n"
"Output each of the arguments to stdout.\n"
"\n"
"~help~\n"
"Output to stdout a brief description of the commands accepted by the shell\n"
"\n"
"~malloc~\n"
"Accepts a single argument which is the number of bytes of memory to be allocated. "
"The number of bytes can be specified either as a decimal integer constant (of arbitrary "
"length), an octal integer constant (indicated by a prefix of 0 not followed by x or "
"X followed by an arbitrary length octal constant), or as a hexadecimal number (indicated "
"by a prefix of 0x or 0X followed by an arbitrary length hexadecimal constant). The "
"alphabetic hexadecimal digits can be specified in either upper or lower case.\n"
"\n"
"~free~\n"
"The free command accepts a single argument which is the address of a region of "
"memory previously allocated using malloc. It accepts the same number formats specified. "
"in the malloc help section: octal, decimal, and hexidecimal.\n"
"\n"
"~memorymap~\n"
"Outputs the map of both allocated and free memory regions\n"
"\n"
"~memset~\n"
"The memset command accepts three arguments. The first is the beginning address of an "
"allocated area of memory, the second is the value to which each byte in the specified "
"memory will be set, and the third is the length(in bytes) of the specified memory. Each "
"of the three arguments can be provided in octal, decimal, or hexidecimal (see malloc).\n"
"\n"
"~memchk~\n"
"The memchk command accepts three arguments. The first is the beginning address of an "
"allocated area of memory, the second is the value to which each byte in the specified "
"memory should be checked against, and the third is the length(in bytes) of the specified "
"memory. Each of the three arguments can be provided in octal, decimal, or hexidecimal (see malloc).\n";


// Maps error codes to error descriptions.
struct error_d
{
    int code;
    char *message;
} error_ds[] = {
    {E_SUCCESS, "No error"},
    {E_NOT_ENOUGH_ARGS, "Not enough arguments provided"},
    {E_TOO_MANY_ARGS, "Too many arguments provided"},
    {E_CMD_NOT_FND, "Please enter a valid command"},
    {E_ARG_TYPE, "Wrong argument type. Run the 'help' command for information about command arguments"},
    {E_WRONG_PID, "The PID of the current process does not match the PID of provided address"},
    {E_ADDR_NOT_ALLOCATED, "The address provided does not match a previously allocated address"},
    {E_MALLOC, "Unable to allocate the requested memory"},
    {E_STRTOUL, "The number you provided is out of range, or contains an invalid character"},
    {E_BRANGE_EX, "The value provided exceeds the storage capacity of a byte"},
    {E_ADDR_SPC, "The range of addresses specified is not within the current address space"}};

// Convenience function to print error codes.
void print_err(int error_c)
{
	int i;
    for (i = 0; i <= E_COUNT; i++)
    {
        if (error_c == error_ds[i].code)
        {
            myprintf("ERROR: %s\n", error_ds[i].message);
            return;
        }
    }
    myprintf("ERROR: The error code returned (%d) doesn't match an enumerated error type\n", error_c);
}

struct commandEntry
{
    char *name;
    int (*functionp)(int argc, char *argv[]);
} commands[] = {
                {"echo", cmd_echo},
                {"exit", cmd_exit},
                {"help", cmd_help},
                {"malloc", cmd_malloc},
                {"free", cmd_free},
                {"memoryMap", cmd_memory_map},
                {"memset", cmd_memset},
                {"memchk", cmd_memchk},
                {"open", cmd_open}
                };

typedef int (*cmd_pntr)(int argc, char *argv[]);


cmd_pntr find_cmd(char *arg)
{
    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
    {
        if (strcmp(arg, commands[i].name) == 0)
        {
            return commands[i].functionp;
        }
    }
    return NULL;
}

#define BUFFER_SIZE_FOR_FORMATTED_OUTPUT 4096

void myprintf(char*format, ...){
    char buffer[BUFFER_SIZE_FOR_FORMATTED_OUTPUT]; // holds the rendered string
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    uartPutsNL(UART2_BASE_PTR, buffer);
}


void initUART(void){
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
	const int IRC = 32000;					/* Internal Reference Clock */
	const int FLL_Factor = 640;
	const int moduleClock = FLL_Factor*IRC;
	const int KHzInHz = 1000;

	const int baud = 9600;
	
	uartInit(UART2_BASE_PTR, moduleClock/KHzInHz, baud);
}

#define BUFFER_SIZE_FOR_SHELL_INPUT 256

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
	initUART();
    if (TEST_MODE) {
        run_test_suite();
    }
    while (1)
    {
        char linebuf[BUFFER_SIZE_FOR_SHELL_INPUT];
        memset(linebuf, 0, BUFFER_SIZE_FOR_SHELL_INPUT);
        myprintf("$ ");
        uartGetline(UART2_BASE_PTR, &linebuf[0], BUFFER_SIZE_FOR_SHELL_INPUT);
        // Iterate over characters in the line buffer and set whitespace to the null terminator.
        for (int i = 0; i < BUFFER_SIZE_FOR_SHELL_INPUT; i++)
        {
        	char c = linebuf[i];
            if (c == ' ' || c == '\t')
            {
                linebuf[i] = 0;
            }
        }
        // Previous character type.
        // 0 = whitespace, 1 = character.
        char pctype = 0;
        // Argument count.
        int argct = 0;
        // Iterate over characters in the line buffer and set argct, argval.
        for (int i = 0; i < BUFFER_SIZE_FOR_SHELL_INPUT; i++)
        {
            // If previous character is a space.
            if (pctype == 0)
            {
                // If current character is also a space.
                if (linebuf[i] == 0)
                {
                    continue;
                }
                // If current character is a character.
                else
                {
                    // Increment arg count, set arg pointer, set prev char type to character.
                    argct++;
                    pctype = 1;
                }
            }
            // If previous character is a character.
            else
            {
                // If current character is a space.
                if (linebuf[i] == 0)
                {
                    // Set prev char type to space.
                    pctype = 0;
                }
            }
        }
        // Initialize argument start indexes array.
        int arglocs[argct];
        // Initialize argument lengths array.
        int arglens[argct];
        int arglocct = 0;
        // Re-initialize previous character type to 0.
        pctype = 0;
        for (int i = 0; i < BUFFER_SIZE_FOR_SHELL_INPUT; i++)
        {
            // If previous character is a space.
            if (pctype == 0)
            {
                // If current character is also a space.
                if (linebuf[i] == 0)
                {
                    continue;
                }
                // If current character is a character.
                else
                {
                    // Set argument start index.
                    arglocs[arglocct] = i;
                    pctype = 1;
                }
            }
            // If previous character is a character.
            else
            {
                // If current character is a space.
                if (linebuf[i] == 0 || linebuf[i] == '\n')
                {
                    // Set argument length.
                    arglens[arglocct] = i - arglocs[arglocct];
                    arglocct++;
                    // Set prev char type to space.
                    pctype = 0;
                }
            }
        }
        // If there are no args (i.e. user pressed enter or space enter) continue before mallocing
        if (argct < 1)
        {
            continue;
        }
        // Allocate space for argval
        char **argval = myMalloc((argct + 1) * sizeof(char *));
        for (int i = 0; i < argct; i++)
        {
            // Assign argvals
            argval[i] = myMalloc((arglens[i] + 1) * sizeof(char));
            // Zero out argval[i] plus a NUL terminator
            myMemset(argval[i], 0, arglens[i] + 1);
            strncpy(argval[i], &linebuf[arglocs[i]], arglens[i]);
        }
        argval[argct] = NULL;
        cmd_pntr shell_cmd = find_cmd(argval[0]);
        if (shell_cmd == NULL)
        {
            print_err(E_CMD_NOT_FND);
        }
        else
        {
            // Only pass the arguments, not the shell command.
            int cmd_c = shell_cmd(argct - 1, &argval[1]);
            // If the command returns a non-zero error code, print the error message.
            if (cmd_c > 0)
            {
                print_err(cmd_c);
            }
        }
        // Free argval[i] and argval
        int i = 0;
        while (i < argct)
        {
            myFree(argval[i]);
            ++i;
        }
        myFree(argval);
    }
}

int cmd_echo(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        // print args.
        myprintf("%s", argv[i]);
        // Here, i is starting at zero and argc is not zero-indexed
        if (i < (argc - 1))
        {
            myprintf("%c", ' ');
        }
        else
        {
            myprintf("%c", '\n');
        }
    }
    return E_SUCCESS;
}

int cmd_exit(int argc, char *argv[])
{
    if (argc > 0)
    {
        return E_TOO_MANY_ARGS;
    }
    if (file_structure_mounted) {
        int unmount = file_structure_umount();
        if (unmount != E_SUCCESS) {
            return unmount;
        }
    }
    exit(E_SUCCESS);
}

int cmd_help(int argc, char *argv[])
{
    if (argc > 0)
    {
        return E_TOO_MANY_ARGS;
    }
    myprintf("%s", help_text);
    return E_SUCCESS;
}

int cmd_malloc(int argc, char *argv[])
{
    if (argc == 0)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    else if (argc > 1)
    {
        return E_TOO_MANY_ARGS;
    }
    long bytes = my_strtol(argv[0]);
    if (bytes < 0)
    {
        return E_STRTOUL;
    }
    // Cast to the type myMalloc is expecting.
    uint32_t c_bytes = (uint32_t)bytes;
    void *p = myMalloc(bytes);
    if (p == NULL)
    {
        return E_MALLOC;
    }
    myprintf("%p\n", p);
    return E_SUCCESS;
}

// Returns -1 if there was an error
long my_strtol(char *str)
{
    errno = 0;
    long bytes = strtol(str, NULL, 0);
    if (errno != 0)
    {
        return -1;
    }
    return bytes;
}

int cmd_free(int argc, char *argv[])
{
    if (argc == 0)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    else if (argc > 1)
    {
        return E_TOO_MANY_ARGS;
    }
    long addr = my_strtol(argv[0]);
    if (addr < 0)
    {
        return E_STRTOUL;
    }
    void *p = (void *)addr;
    int free_status = myFreeErrorCode(p);
    if (free_status == 0)
    {
        myprintf("Memory address %p successfully freed\n", p);
    }
    return free_status;
}

int cmd_memory_map(int argc, char *argv[]) {
    if (argc > 0)
    {
        return E_TOO_MANY_ARGS;
    }
    memoryMap();
    return E_SUCCESS;
}

int cmd_memset(int argc, char *argv[])
{
    if (argc < 3)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    else if (argc > 3)
    {
        return E_TOO_MANY_ARGS;
    }
    long start_addr = my_strtol(argv[0]);
    if (start_addr < 0)
    {
        return E_STRTOUL;
    }
    void *start_p = (void *)start_addr;
    long byte_val = my_strtol(argv[1]);
    if (byte_val < 0)
    {
        return E_STRTOUL;
    } else if (byte_val > 255) {
        return E_BRANGE_EX;
    }
    long size = my_strtol(argv[2]);
    if (size < 0)
    {
        return E_STRTOUL;
    }
    return myMemset(start_p, (uint8_t)byte_val, size);
}

int cmd_memchk(int argc, char *argv[])
{
    if (argc < 3)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    else if (argc > 3)
    {
        return E_TOO_MANY_ARGS;
    }
    long start_addr = my_strtol(argv[0]);
    if (start_addr < 0)
    {
        return E_STRTOUL;
    }
    void *start_p = (void *)start_addr;
    long byte_val = my_strtol(argv[1]);
    if (byte_val < 0)
    {
        return E_STRTOUL;
    }
    else if (byte_val > 255)
    {
        return E_BRANGE_EX;
    }
    long size = my_strtol(argv[2]);
    if (size < 0)
    {
        return E_STRTOUL;
    }
    return myMemchk(start_p, (uint8_t)byte_val, size);
}

/**
 * Shell "create" command
 */
int cmd_create(int argc, char *argv[])
{
    if (argc != 1)
    {
        return E_NOT_ENOUGH_ARGS;
    }

    return E_SUCCESS;
}

/**
 * Shell "open" command
 */
int cmd_open(int argc, char *argv[])
{
    if (argc != 1)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    file_descriptor *fd = myMalloc(sizeof(file_descriptor));
    int open_status = fopen(arg[0], fd);
    if (open_status != E_SUCCESS) {
        return E_FILE_OPEN;
    }
    myprintf("%lu\n", (unsigned long)*file_descriptor)
    return E_SUCCESS;
}

/**
 * Shell "close" command
 */
int cmd_close(int argc, char *argv[])
{
    if (argc != 1)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    return E_SUCCESS;
}

/**
 * Shell "read" command
 */
int cmd_read(int argc, char *argv[])
{
    if (argc != 1)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    return E_SUCCESS;
}

/**
 * Shell "write" command
 */
int cmd_write(int argc, char *argv[])
{
    if (argc != 1)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    return E_SUCCESS;
}

