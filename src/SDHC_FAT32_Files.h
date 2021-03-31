#ifndef _SDHC_FAT32_FILES_H_
#define _SDHC_FAT32_FILES_H_

#include <stdint.h>
#include "directory.h"
#include "bootSector.h"
#include "devinio.h"

/* All functions return an int which indicates success if 0 and an
   error code otherwise (only some errors are listed) */

/**
 * Indicates that the microSDHC card is to be made available for these
 * function calls
 *
 * Actions this function should take: 
 * - call microSDCardDetectConfig
 * - check to see if a microSDHC card is present
 * - if not present return an error; 
 * - otherwise, call microSDCardDisableCardDetectARMPullDownResistor to disable
 *   the pull-down resistor in the K70 ARM
 * - call sdhc_initialize
 * Returns an error code if the file structure is already mounted
 */
int file_structure_mount(void);

/**
 * Indicates that the microSDHC card is no longer accessible
 * Action this function should take: 
 * - call sdhc_command_send_set_clr_card_detect_connect to re-enable the
 *   card-present-resistor
 * Returns an error code if the file structure is not mounted
 */
int file_structure_umount(void);

/**
 * Sets the cwd to the root directory
 * This is the initial action before the FAT32 file structure is made
 * available through these interfaces
 */
int dir_set_cwd_to_root(void);

/**
 * Display on stdout the cwd's filenames (full == 0) or all directory
 * information (full == 1); implementing full is optional
 */
int dir_ls(void);

/**
 * Start an iterator at the beginning of the cwd's filenames
 * Returns in *statepp a pointer to a malloc'ed struct that contains necessary
 * state for the iterator
 * Optional with dir_ls_next
 */
int dir_ls_init(void **statepp);

/**
 * Uses statep as a pointer to a struct malloc'ed and initialized by
 * dir_ls_init that contains iterator state
 * Returns in filename the malloc'ed name of the next filename in the cwd; 
 * Caller is responsible to free the memory
 * Returns NULL for filename if at end of the directory; If returning NULL,
 * the malloc'ed struct pointed to by statep is free'd
 * Optional with dir_ls_init
 */
int dir_ls_next(void *statep, char *filename);

/**
 * Search for filename in cwd and return its first cluster number in
 * firstCluster
 * Returns an error code if filename is not in the cwd
 * Returns an error code if the filename is a directory
 */
int dir_find_file(char *filename, uint32_t *firstCluster);

/**
 * Search for filename in cwd and, if it is a directory, set the cwd to that
 * filename
 * Returns an error code if filename is not in the cwd
 * Returns an error code if the filename is not a directory
 * Implementing this function is optional
 */
int dir_set_cwd_to_filename(char *filename);

/**
 * Create a new empty regular file in the cwd with name filename
 * Returns an error code if a regular file or directory already exists with
 * this filename
 * Returns an error code if there is no more space to create the regular file
 */
int dir_create_file(char *filename);

/**
 * Delete a regular file in the cwd with name filename
 * Returns an error code if a file with this name does not exist
 * Returns an error code if a file with this filename is currently open
 * Returns an error code if the file with this name is a directory
 */
int dir_delete_file(char *filename);

/**
 * Create a new empty directory in the cwd with name filename
 * Returns an error code if there is no more space to create the directory
 * Implementing this function is optional
 */
int dir_create_dir(char *filename);

/**
 * Delete a directory in the cwd with name filename
 * Returns an error code if a file with this name does not exist
 * Returns an error code if a directory with this filename contains any files
 * or directories
 * Returns an error code if the file with this name is not a directory
 * Implementing this function is optional
 */
int dir_delete_dir(char *filename);

/**
 * Moved file_descriptor declaration to devinio.h
 */

/**
 * Search for filename in the cwd and, if successful, store a file descriptor
 * for that file into *descrp
 * Returns an error code if filename is not in the cwd
 * Returns an error code if the filename is not a regular file
 */
int file_open(char *filename, file_descriptor *descrp);

/**
 * Close the file associated with descr and disassociate descr from that file
 * Returns an error code if the file descriptor is not open
 * Frees all dynamic storage associated with the formerly open file descriptor
 * and indicates that the descriptor is closed
 */
int file_close(file_descriptor descr);

/**
 * Read sequential characters at the current offset from the file associated
 * with descr into the buffer pointed to by bufp for a maximum length of buflen
 * characters; The actual number of characters read is returned in the int
 * pointed to by charsreadp
 * Returns an error code if the file descriptor is not open
 * Returns an error code if there are no more characters to be read from the
 * file (EOF, this is, End Of File)
 */
int file_getbuf(file_descriptor descr, char *bufp, int buflen, int *charsreadp);

/**
 * Write characters at the current offset into the file associated with descr
 * from the buffer pointed to by bufp with a length of buflen characters
 * Returns an error code if the file descriptor is not open
 * Returns an error code if the file is not writeable
 * (See DIR_ENTRY_ATTR_READ_ONLY), optional to implement read-only
 * Returns an error code if there is no more space to write the character
 */
int file_putbuf(file_descriptor descr, char *bufp, int buflen);


/////// ADDED BY JAMES

/**
 * Cluster number of the current working directory.
 */
extern uint32_t cwd;

/**
 * Takes a character and determines if it's a valid FAT32 8_3 filename character
 */
int chr_8_3_valid(uint8_t c);

/**
 * A wrapper for a dir entry filename
 */
typedef struct filename_8_3_wrapper {
   uint8_t name[9];
   uint8_t ext[4];
   uint8_t combined[13];
} Filename_8_3_Wrapper;

/**
 * Takes a *dir_entry_8_3->Dir_Name and and  uses it to populate a *Filename_8_3_Wrapper.
 * Param dir_entry: pointer to a valid FAT32 directory entry
 * Param file_name: pointer to a File_8_3_Wrapper
 * Returns: E_SUCCESS if all went well
 * Error: If an illegal character is detected at the beginning of the filename the function
 * returns E_FILE_NAME_INVALID
 */ 
int entry_to_filename(struct dir_entry_8_3 *dir_entry, Filename_8_3_Wrapper *file_wrapper);

/**
 * Takes an *Filename_8_3_Wrapper and  uses it to populate a *dir_entry_8_3->Dir_Name.
 * Param file_name: pointer to a File_8_3_Wrapper
 * Param dir_entry: pointer to a valid FAT32 directory entry
 * Returns: E_SUCCESS if all went well
 * Error: If an illegal character is detected in the Filename_8_3_Wrapper the function
 * returns E_FILE_NAME_INVALID
 */ 
int filename_to_entry(Filename_8_3_Wrapper *file_wrapper, struct dir_entry_8_3 *dir_entry);

int create_filename_wrapper(char *filename, Filename_8_3_Wrapper *file_wrapper);

/**
 * Extended dir_find_file
 * The same as dir_find_file except it places a pointer to the dir entry in dir_entry if the file is found
 */
int dir_find_file_x(char *filename, uint32_t *firstCluster, struct dir_entry_8_3 *dir_entry);

#endif /* ifndef _SDHC_FAT32_FILES_H_ */
