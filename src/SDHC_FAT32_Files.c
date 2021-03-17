#include "SDHC_FAT32_Files.h"
#include "shell.h"
#include "microSD.h"

int file_structure_mounted;

uint32_t rca;

int file_structure_mount(void) {
    if (file_structure_mounted) {
        __BKPT();
        return E_FILE_STRUCT_MOUNTED;
    }
    microSDCardDetectConfig();
    int microSDdetected = microSDCardDetectedUsingSwitch();
    if (!microSDdetected) {
        __BKPT();
        return E_NO_MICRO_SD;
    }
    microSDCardDisableCardDetectARMPullDownResistor();
    rca = sdhc_initialize();
    file_structure_mounted = 1;
    return E_SUCCESS;
}

int file_structure_umount(void) {
    if (!file_structure_mounted) {
        __BKPT();
        return E_FILE_STRUCT_NOT_MOUNTED;
    }
    sdhc_command_send_set_clr_card_detect_connect(rca);
    return E_SUCCESS

}