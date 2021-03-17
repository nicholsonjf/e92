#include "SDHC_FAT32_Files.h"
#include "shell.h"
#include "microSD.h"

int file_structure_mounted;

int file_structure_mount(void) {
    microSDCardDetectConfig();
    int microSDdetected = microSDCardDetectedUsingSwitch();
    if (!microSDdetected) {
        return E_NO_MICRO_SD;
    }
    microSDCardDisableCardDetectARMPullDownResistor();
    sdhc_initialize();
    return E_SUCCESS;
}