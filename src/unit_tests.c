#include "unit_tests.h"
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
#include "utils.h"


int debug = 1;

int test_create_file(void) {
    char *test_name = "Create File";
    char *result = "FAIL";
    char *fname = "TEST.JPG";
    int delete_file = dir_delete_file(fname);
    if (delete_file != E_SUCCESS || E_FILE_NOT_IN_CWD) {
        return delete_file;
    }
    int create_file = dir_create_file(fname);
    if (create_file == E_SUCCESS) {
        result = "PASS";
    }
    myprintf("%s: %s\n\n", test_name, result);
    if (debug == 1) {
        myprintf("delete_file error code: %d\n", delete_file);
        myprintf("create_file error code: %d\n", create_file);
        dir_set_cwd_to_root();
        dir_ls();
    }
    return E_SUCCESS;
}

int run_test_suite() {
    test_create_file();
    return E_SUCCESS;
}