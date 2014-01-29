#include "fs_lib.h"
#include "stdio.h"
#include "fcntl.h"

int test_read(int srvhndl) {
  int fd, i;
  char buffer[1024];
  int flags;
  flags = O_RDWR;
  printf("\n-----\ntest_read() begin\n-----\n");

  printf("\nOpening testm.txt %d...",flags);
  fs_open(srvhndl, "testm.txt",flags, &fd);
  printf("DONE\n");
  printf("\nRead 5 chars...");
  fs_read(srvhndl, fd, buffer,5);
  printf("DONE\n");
  printf("\nClose testm.txt...");
  fs_close(srvhndl, fd);
  printf("DONE\n");

  for(i=0; i<5; i++){
    printf("%c", buffer[i]);
  }

  printf("\n-----\ntest_read() end\n-----\n");
  fflush(stdout);
}

int test_open(int srvhndl) {
  int fd, i;
  char buffer[1024];
  int flags;
  flags = O_RDWR;
  printf("\n-----\ntest_open() begin\n-----\n");

  printf("\nOpening testm.txt %d...",flags);
  fs_open(srvhndl, "testm.txt",flags, &fd);
  printf("DONE\n");
  printf("\nClose testm.txt...");
  fs_close(srvhndl, fd);
  printf("DONE\n");

  for(i=0; i<5; i++){
    printf("%c", buffer[i]);
  }

  printf("\n-----\ntest_open() end\n-----\n");
  fflush(stdout);
}

int test_write(int srvhndl) {
  int fd, i;
  int flags;
  flags = O_WRONLY | O_CREAT;
  printf("\n-----\ntest_write() begin\n-----\n");

  fs_open(srvhndl, "testm.txt",flags, &fd);
  fs_write(srvhndl, fd, "Hello world!",5);
  fs_close(srvhndl, fd);

  printf("\n-----\ntest_write() end\n-----\n");
  fflush(stdout);
}

int test_lseek(int srvhndl) {
  int fd, i;
  char buffer[1024];
  int flags;
  flags = O_RDWR;
  printf("\n-----\ntest_lseek() begin\n-----\n");
  buffer[0]="1111111111111\0";

  printf("\nOpening testm.txt %d...",flags);
  fs_open(srvhndl, "testm.txt",flags, &fd);
  printf("DONE\n");
  printf("\nRead 5 chars...");
  fs_read(srvhndl, fd, buffer,5);
  printf("DONE\n");
  printf("\nMove pointer to position 3...");
  fflush(stdout);
  fs_lseek(srvhndl, fd, 3,SEEK_SET);
  printf("DONE\n");
  fflush(stdout);
  printf("\nRead 5 chars...");
  fflush(stdout);
  fs_read(srvhndl, fd, &buffer[5],5);
  printf("DONE\n");
  fflush(stdout);
  printf("\nClose testm.txt...");
  fs_close(srvhndl, fd);
  printf("DONE\n");
  fflush(stdout);

  printf("\nResults:\n");
  for(i=0; i<10; i++){
    printf("%c", buffer[i]);
  }
  printf("\n");
  printf("\n-----\ntest_lseek() end\n-----\n");
  fflush(stdout);
}

int main(int argc, char **argv){
  int srvhndl;


  int port=0;
  char protocol[4]="XXX";
  if(argc<4){
          printf("Usage:\n./testy <address> <protocol> <port>\nwhere:\n<protocol> - TCP or UDP\n<port>     - port number\n");
          return 0;
  }else{
            memcpy(&protocol, argv[2], 4);
            port=strtol(argv[3], NULL, 10);
            if(strcmp(protocol,"TCP")!=0 && strcmp(protocol,"UDP")!=0){
                    printf("Wrong protocol specified.\n");
                    return 0;
            }
            printf("Running with %s connection on port %d...\n",protocol,port);
  }

  fs_openserver("127.0.0.1", protocol, port, &srvhndl);


  test_open(srvhndl);
  test_lseek(srvhndl);
  test_write(srvhndl);
  test_read(srvhndl);
  return 0;
}
