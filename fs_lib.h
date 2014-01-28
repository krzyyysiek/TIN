#ifndef _FS_LIB
#define _FS_LIB
#include <sys/types.h>

int fs_openserver(char *ip, char protocol[4], int port, int *srvhndl_out);
int fs_closeserver(int srvhndl);
int fs_open(int srvhndl, char *path,int flags, int *fd_out);
int fs_close(int srvhndl, int fd);
int fs_read(int srvhndl, int fd, char *buffer, int len);
int fs_write(int srvnhld, int fd, char *ptr, int len);
off_t fs_lseek(int srvhndl, int fd, off_t offset, int whence);
int fs_fstat(int srvhndl, int fd, struct stat *buf);

#endif
