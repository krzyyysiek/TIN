#ifndef _FS_LIB
#define _FS_LIB

int fs_openserver(char *ip, char protocol[4], int port, int *srvhndl_out);
int fs_closeserver(int srvhndl);
int fs_open(int srvhndl, char *path, int *fd_out);
int fs_close(int srvhndl, int fd);
int fs_read(int srvhndl, int fd, char *buffer, int len);
int fs_write(int srvnhld, int fd, char *ptr, int len);

#endif
