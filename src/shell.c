#include "shell.h"
#include "my-malloc.h"
#include "uart.h"
#include "mcg.h"
#include "delay.h"
#include "uartNL.h"
#include "derivative.h"
#include "devinio.h"
#include "breakpoint.h"
#include "unit_tests.h"
#include "utils.h"
#include "devinit.h"
#include "devinutils.h"
#include "SDHC_FAT32_Files.h"
#include "sdram.h"
#include "svc.h"
#include "priv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>


char *help_text =
"\n"
"**********************\n"
"*** JELL HELP TEXT ***\n"
"**********************\n"
"\n"
"Help text format:\n"
"command [arg]...\n"
"Help text...\n"
"\n"
"All arguments are required unless stated otherwise.\n"
"\n"
"\n"
"exit\n"
"Exit from the shell(i.e., cause the shell to terminate).\n"
"\n"
"echo [text]\n"
"Outputs [text] to stdout. Max input size is 512 characters.\n"
"\n"
"help\n"
"Output this help text to stdout.\n"
"\n"
"malloc [address]\n"
"Accepts a single argument [address] which is the number of bytes of memory to be allocated. "
"The number of bytes can be specified either as a decimal integer constant (of arbitrary "
"length), an octal integer constant (indicated by a prefix of 0 not followed by x or "
"X followed by an arbitrary length octal constant), or as a hexadecimal number (indicated "
"by a prefix of 0x or 0X followed by an arbitrary length hexadecimal constant). The "
"alphabetic hexadecimal digits can be specified in either upper or lower case.\n"
"\n"
"free [address]\n"
"The free command accepts a single argument [address] which is the address of a region of "
"memory previously allocated using malloc. It accepts the same number formats specified. "
"in the malloc help section: octal, decimal, and hexidecimal.\n"
"\n"
"memorymap\n"
"Outputs the map of both allocated and free memory regions\n"
"\n"
"memset [start_address] [mem_value] [length]\n"
"The memset command accepts three arguments. The first [start_address] is the beginning address of an "
"allocated area of memory, the second [mem_value] is the value to which each byte in the specified "
"memory will be set, and the third [length] is the length(in bytes) of the specified memory. Each "
"of the three arguments can be provided in octal, decimal, or hexidecimal (see malloc).\n"
"\n"
"memchk [start_address] [mem_value] [length]\n"
"The memchk command accepts three arguments. The first [start_address] is the beginning address of an "
"allocated area of memory, the second [mem_value] is the value to which each byte in the specified "
"memory should be checked against, and the third [length] is the length(in bytes) of the specified "
"memory. Each of the three arguments can be provided in octal, decimal, or hexidecimal (see malloc).\n"
"\n"
"open [path]\n"
"Opens the file located at [path] and provides a file descriptor that can be used to reference the "
"open file in other operations. [path] must be the absolute path of the file, i.e. /MYFILE.TXT. "
"See the DEVICES section below for more information.\n"
"\n"
"create [path]\n"
"Creates a new file in the mounted filesystem. [path] must be the absolute path of the file, i.e. /MYFILE.TXT. "
"The file must be opened to get a file descriptor.\n"
"\n"
"read [file descriptor] [number of characters]\n"
"Reads [number of characters] from the file located at [file descriptor] and prints them to the "
"configured UART channel. The maximum value for [number of characters] is 512.\n"
"\n"
"write [file descriptor] [text]\n"
"Writes [text] to the file located at [file descriptor]. The number of characters that can be provided "
"in a single call to write is 512.\n"
"\n"
"close [file descriptor]\n"
"Closes the file located at [file descriptor].\n"
"\n"
"delete [path]\n"
"Deletes the filesystem file located at [path]. [path] must be the absolute path of the file, i.e. /MYFILE.TXT.\n"
"\n"
"ls\n"
"List all the files in the current directory.\n"

"DEVICES:\n"
"\n"
"FAT32\n"
"To open FAT32 files the [path] is the a forward slash followed by the filename:\n"
"\n"
"/MYFILE.TXT\n"
"\n"
"Only short 8.3 filenames are accepted: 8 characters max for the filename and 3 characters max for the extension.\n"
"FAT32 filenames must be all uppercase.\n"
"\n"
"LEDS\n"
"There are four LED lights mounted to the OS at startup, which can be accessed via their file paths.\n"
"\n"
"/dev/ledy\n"
"/dev/ledg\n"
"/dev/ledo\n"
"/dev/ledb\n"
"\n"
"To turn on an LED light, open it, and then write 'on' to its file descriptor:\n"
"\n"
"$ open /dev/ledb\n"
"0\n"
"$ write 0 on\n"
"\n"
"To turn it off, read zero characters from its file descriptor:"
"\n"
"$ read 0 0\n"
"\n"
"PUSH BUTTONS\n"
"There are four two push buttons mounted to the OS at startup, which can be accessed via their file paths.\n"
"\n"
"/dev/sw1\n"
"/dev/sw2\n"
"\n"
"To check the status of a push button, open it and then read 1 character form its file descriptor:\n"
"\n"
"$ open /dev/sw2\n"
"3\n"
"$ read 3 1\n"
"\n"
"An integer will be printed to UART that corresponds to the state of the pushbutton at the moment the command was run:\n"
"\n"
"0 - no change\n"
"1 - sw1 is down\n"
"2 - sw1 is up\n"
"3 - sw2 is down\n"
"4 - sw2 is up\n"
"\n";

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
    {E_STRTOL, "The number you provided is out of range, or contains an invalid character"},
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
    myprintf("ERROR (%d): see utils.h for enum value that can be used to search source code. Will add error text in a future PR.\n", error_c);
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
    {"open", cmd_open},
    {"close", cmd_close},
    {"create", cmd_create},
    {"read", cmd_read},
    {"write", cmd_write},
    {"delete", cmd_delete},
    {"ls", cmd_ls}
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
	const int moduleClock = 60000000;
	const int KHzInHz = 1000;
    const int baud = 115200;
    uartInit(UART2_BASE_PTR, moduleClock / KHzInHz, baud);
}

#define BUFFER_SIZE_FOR_SHELL_INPUT 256

int shell(int argc, char **argv)
{
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
    int exit_devices_status = exitDevices();
    if (exit_devices_status != E_SUCCESS) {
        return exit_devices_status;
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
    unsigned long bytes = my_strtoul(argv[0]);
    if (bytes < 0)
    {
        return E_STRTOL;
    }
    void *p = myMalloc(bytes);
    if (p == NULL)
    {
        return E_MALLOC;
    }
    myprintf("%p\n", p);
    return E_SUCCESS;
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
    unsigned long addr = my_strtoul(argv[0]);
    if (addr < 0)
    {
        return E_STRTOL;
    }
    void *p = (void *)addr;
    int free_status = SVCMyfree(p);
    if (free_status != E_SUCCESS)
    {
        return free_status;
    }
    myprintf("Memory address %p successfully freed\n", p);
    return E_SUCCESS;
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
    unsigned long start_addr = my_strtoul(argv[0]);
    if (start_addr < 0)
    {
        return E_STRTOL;
    }
    void *start_p = (void *)start_addr;
    unsigned long byte_val = my_strtoul(argv[1]);
    if (byte_val < 0)
    {
        return E_STRTOL;
    } else if (byte_val > 255) {
        return E_BRANGE_EX;
    }
    unsigned long size = my_strtoul(argv[2]);
    if (size < 0)
    {
        return E_STRTOL;
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
    unsigned long start_addr = my_strtoul(argv[0]);
    if (start_addr < 0)
    {
        return E_STRTOL;
    }
    void *start_p = (void *)start_addr;
    unsigned long byte_val = my_strtoul(argv[1]);
    if (byte_val < 0)
    {
        return E_STRTOL;
    }
    else if (byte_val > 255)
    {
        return E_BRANGE_EX;
    }
    unsigned long size = my_strtoul(argv[2]);
    if (size < 0)
    {
        return E_STRTOL;
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
    int create_file_status = SVCMyfcreate(argv[0]);
    if (create_file_status != E_SUCCESS)
    {
        return create_file_status;
    }
    return E_SUCCESS;
}

/**
 * Shell "delete" command
 */
int cmd_delete(int argc, char *argv[])
{
    if (argc != 1)
    {
        return E_NOT_ENOUGH_ARGS;
    }

    int delete_status = SVCMyfdelete(argv[0]);
    if (delete_status != E_SUCCESS)
    {
        return delete_status;
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
    file_descriptor fd;
    int open_status = SVCMyfopen(argv[0], &fd);
    if (open_status != E_SUCCESS) {
        return open_status;
    }
    unsigned long fd_index = (unsigned long)fd;
    myprintf("%lu\n", fd_index);
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
    unsigned long fd_long = my_strtoul(argv[0]);
    if (fd_long < 0)
    {
        return E_STRTOL;
    }
    file_descriptor fd = (file_descriptor)fd_long;
    int close_status = SVCMyfclose(&fd);
    if (close_status != E_SUCCESS)
    {
        return close_status;
    }
    return E_SUCCESS;
}

/**
 * Shell "read" command
 */
int cmd_read(int argc, char *argv[])
{
    if (argc < 2)
    {
        return E_NOT_ENOUGH_ARGS;
    }
    unsigned long fd_long = my_strtoul(argv[0]);
    if (fd_long < 0)
    {
        return E_STRTOL;
    }
    file_descriptor fd = (file_descriptor)fd_long;
    unsigned long num_chars_req = my_strtoul(argv[1]);
    if (num_chars_req > 512) {
        return E_READ_LIMIT;
    }
    int num_chars_act = 0;
    char raw_file_chars[512];
    char clean_file_chars[512];
    int get_buf_status = SVCMyfgetc(fd, &raw_file_chars[0], num_chars_req, &num_chars_act);
    if (get_buf_status != E_SUCCESS) {
        return get_buf_status;
    }
    // Clean the read characters before printing
    int char_wash_status = char_wash(&raw_file_chars[0], num_chars_act, &clean_file_chars[0]);
    if (char_wash_status != E_SUCCESS) {
        return char_wash_status;
    }
    myprintf("%s\n", clean_file_chars);
    return E_SUCCESS;
}

/**
 * Shell "write" command
 */
int cmd_write(int argc, char *argv[])
{
    if (argc < 2) {
        return E_NOT_ENOUGH_ARGS;
    }
    unsigned long fd_long = my_strtoul(argv[0]);
    if (fd_long < 0)
    {
        return E_STRTOL;
    }
    file_descriptor fd = (file_descriptor)fd_long;
    char buffer[BUFFER_SIZE_FOR_SHELL_INPUT]; // holds the rendered string
    int bufpos = 0;
    for (int i=1; i<argc; i++) {
        int j = 0;
        while(argv[i][j] != 0) {
            buffer[bufpos] = argv[i][j];
            bufpos++;
            j++;
        }
        // Space in between words, excluding after the last word
        if (i < argc-1) {
            buffer[bufpos] = ' ';
            bufpos++;
        }
    }
    // Null terminate the buffer
    buffer[bufpos] = 0;
    if (strlen(&buffer[0]) > 512) {
        return E_WRITE_LIMIT;
    }
    // bufpos+1 to include the null terminator
    int write_status = SVCMyfputc(&fd, &buffer[0], bufpos+1);
    if (write_status != E_SUCCESS) {
    	return write_status;
    }
    return E_SUCCESS;
}

/**
 * Shell "ls" command
 */
int cmd_ls(int argc, char *argv[])
{
    if (argc > 0)
    {
        return E_TOO_MANY_ARGS;
    }
    int ls_status = SVCMydir_ls();
    if (ls_status != E_SUCCESS) {
        return E_LS;
    }
    return E_SUCCESS;
}

int main(int argc, char **argv)
{
    mcgInit();
    setvbuf(stdout, NULL, _IONBF, 0);
    initUART();
    sdramInit();
    initDevices();
    if (TEST_MODE)
    {
        run_test_suite();
    }
    privUnprivileged();
    shell(argc, argv);
}