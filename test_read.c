#include "fs_lib.h"
#include "stdio.h"
#include "fcntl.h"

int test_read(int srvhndl, char *filename, int len) {
  int fd, i;
  char buffer[len];
  int flags;
  flags = O_RDWR;
  printf("\n-----\ntest_read() begin\n-----\n\n");

  	  	  printf("Opening file %s...",filename);
  fs_open(srvhndl, filename,flags, &fd);
  	  	  printf("DONE\n");

  	  	  printf("Read %d chars...", len);
  fs_read(srvhndl, fd, buffer,len);
  	  	  printf("DONE\n");

  	  	  printf("Close %s...",filename);
  fs_close(srvhndl, fd);
  	  	  printf("DONE\n");

  printf("Results:\n");
  for(i=0; i<len; i++){
    printf("%c", buffer[i]);
  }

  printf("\n\n-----\ntest_read() end\n-----\n");
  fflush(stdout);
}

int main(int argc, char **argv){
  int srvhndl;
  int port=0,len=0;
  char protocol[4]="XXX";

  if(argc<5){
          printf("Usage:\n./test_read <address> <protocol> <port> <filename>\nwhere:\n<protocol> - TCP or UDP\n<port>     - port number\n<filename> - file to open\n<number>   - length to read\n");
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
  len=strtol(argv[5], NULL, 10);
  fs_openserver("127.0.0.1", protocol, port, &srvhndl);
  test_read(srvhndl,argv[4],len);
  return 0;
}
