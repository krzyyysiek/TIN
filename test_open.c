#include "fs_lib.h"
#include "stdio.h"
#include "fcntl.h"

int test_open(int srvhndl, char *filename) {
  int fd, i;
  int flags;
  flags = O_RDWR;
  printf("\n----- test_open() begin -----\n\n");

  	  	  printf("Opening file %s...",filename);
  fs_open(srvhndl,filename, flags, &fd);
  	  	  printf("DONE\n");

  printf("\n\n----- test_open() end -----\n");
  fflush(stdout);
}

int main(int argc, char **argv){
  int srvhndl;
  int port=0;
  char protocol[4]="XXX";

  if(argc<5){
          printf("Usage:\n./test_open <address> <protocol> <port> <filename>\nwhere:\n<protocol> - TCP or UDP\n<port>     - port number\n<filename> - file to open\n");
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
  test_open(srvhndl,argv[4]);
  return 0;
}
