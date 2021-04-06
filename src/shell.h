#ifndef _MYSHELL_H
#define _MYSHELL_H

// Shell command function prototypes.
int cmd_date(int argc, char *argv[]);
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
int cmd_clockdate(int argc, char *argv[]);
int cmd_malloc(int argc, char *argv[]);
int cmd_free(int argc, char *argv[]);
int cmd_memory_map(int argc, char *argv[]);
int cmd_memset(int argc, char *argv[]);
int cmd_memchk(int argc, char *argv[]);
int cmd_open(int argc, char *argv[]);
int cmd_create(int argc, char *argv[]);
int cmd_read(int argc, char *argv[]);
int cmd_write(int argc, char *argv[]);
int cmd_ls(int argc, char *argv[]);
int cmd_delete(int argc, char *argv[]);
int cmd_close(int argc, char *argv[]);

#endif /* ifndef _MYSHELL_H */