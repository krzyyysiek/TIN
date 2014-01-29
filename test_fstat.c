#include "fs_lib.h"
#include "stdio.h"
#include "fcntl.h"

int test_fstat(int srvhndl,char *filename) {
  int fd, i;
  int flags = O_RDWR;
  struct stat buf;
  printf("\n----- test_fstat() begin -----\n");

  	  	  printf("Opening file %s...",filename);
  fs_open(srvhndl, filename,flags, &fd);
	  	  printf("DONE\n");
	  	  fflush(stdout);

  fs_fstat(srvhndl, fd, &buf);

  	  	  printf("File size: %d\n",buf.st_size);

  	  	  printf("Close %s...",filename);
  fs_close(srvhndl, fd);
	  	  printf("DONE\n");

  printf("\n----- test_fstat() end -----\n");
  fflush(stdout);
}

int main(int argc, char **argv){
  int srvhndl;
  int port=0;
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
  fs_openserver("127.0.0.1", protocol, port, &srvhndl);
  test_fstat(srvhndl,argv[4]);
  return 0;
}
