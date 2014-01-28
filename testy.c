#include "fs_lib.h"
#include "stdio.h"
#include "fcntl.h"

int test_write() {
  int srvhndl, fd, i, flags;
  char *buffer = "wololo"; 

  flags = O_APPEND | O_WRONLY | O_CREAT;

  fs_openserver("127.0.0.1", "TCP", 9000, &srvhndl);
  fs_open(srvhndl, "test.txt",flags, &fd);
  fs_write(srvhndl, fd, buffer,5);
  fs_close(srvhndl, fd);
  fflush(stdout);
}

int test_read() {
  int srvhndl, fd, i, flags;
  char buffer[1024];

  flags = O_RDWR;

  fs_openserver("127.0.0.1", "TCP", 9000, &srvhndl);
  fs_open(srvhndl, "test.txt",flags, &fd);
  fs_read(srvhndl, fd, buffer,5);
  for(i=0; i<5; i++){
    printf("%c", buffer[i]);
    fflush(stdout);
  }
  printf("\n");
  fflush(stdout);
}

int main(int argc, char **argv){
  test_read();
  //test_write();
  return 0;
}
