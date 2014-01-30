#include "fs_lib.h"
#include "stdio.h"
#include "fcntl.h"

int test_write(int srvhndl, char *filename, char *buffer) {
  int fd, i;
  int flags;
  flags = O_WRONLY | O_CREAT;
  printf("\n----- test_write() begin -----\n\n");

  	  	  printf("Opening file %s...",filename);
  fs_open(srvhndl, filename,flags, &fd);
  	  	  printf("DONE\n");

		  printf("Write %d chars...",strlen(buffer));
  fs_write(srvhndl, fd, buffer,strlen(buffer));
  	  	  printf("DONE\n");

		  printf("Close %s...",filename);
  fs_close(srvhndl, fd);
  	  	  printf("DONE");

  printf("\n\n----- test_write() end -----\n");
  fflush(stdout);
}

int main(int argc, char **argv){
	  int srvhndl;
	  int port=0;
	  char protocol[4]="XXX";

	  if(argc<5){
	          printf("Usage:\n./test_write <address> <protocol> <port> <filename>\nwhere:\n<protocol> - TCP or UDP\n<port>     - port number\n<filename> - file to write\n<content>  - content to write\n");
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
	  fs_openserver(argv[1], protocol, port, &srvhndl);
	  test_write(srvhndl,argv[4],argv[5]);
	  return 0;
}
