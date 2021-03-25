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


int debug = 1;

uint8_t rchar(void) {
    return 'A' + (rand() % 26);
}

void test_create_file(void) {
    char *test_name = "Create File";
    char *result = "FAIL";
    char fname[4] = {rchar(), '.', rchar(), '\0'};
    int create_file = dir_create_file(fname);
    if (create_file == E_SUCCESS) {
        result = "PASS";
    }
    myprintf("%s: %s\n\n", test_name, result);
    if (debug == 1) {
        myprintf("create_file error code: %d\n", create_file);
        myprintf("random file name: %s\n", fname);
        dir_set_cwd_to_root();
        dir_ls();
    }
}

void run_test_suite() {
    initDevIO();
    test_create_file();
}