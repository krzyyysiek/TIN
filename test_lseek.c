#include "fs_lib.h"
#include "stdio.h"
#include "fcntl.h"

int test_lseek(int srvhndl, char *filename, int offset, char *whence) {
  int fd, i;
  char buffer[6];
  char buffer2[6];
  int flags;
  flags = O_RDWR;
  printf("\n----- test_lseek() begin -----\n\n");
  buffer[0]="1111111111111\0";

  	  	  printf("Opening file %s...",filename);
  fs_open(srvhndl, "testm.txt",flags, &fd);
  	  	  printf("DONE\n");

		  printf("Read 5 chars...");
                  fflush(stdout);
  fs_read(srvhndl, fd, buffer,5);
  	  	  printf("DONE\n"); fflush(stdout);

  if(strcmp(whence,"SEEK_SET")){
		  printf("Move pointer to position %d from begin...",offset);
  fs_lseek(srvhndl, fd, offset,SEEK_SET);
  	  	  printf("DONE\n");
  }else if(strcmp(whence,"SEEK_CUR")){
		  printf("Move pointer to position %d from current...",offset);
  fs_lseek(srvhndl, fd, offset,SEEK_CUR);
  	  	  printf("DONE\n");
  }else if(strcmp(whence,"SEEK_END")){
		  printf("Move pointer to position %d from end...",offset);
  fs_lseek(srvhndl, fd, offset,SEEK_END);
  	  	  printf("DONE\n");
  }else{
  		  printf("Bad <whence> value.");
  }

		  printf("Read 5 chars..."); fflush(stdout);
  fs_read(srvhndl, fd, &buffer2,5);
  	  	  printf("DONE\n"); fflush(stdout);

  	  	  printf("Close %s...",filename); fflush(stdout);
  fs_close(srvhndl, fd);
  	  	  printf("DONE\n"); fflush(stdout);
  buffer[5]='\0';
  buffer2[5]='\0';
  printf("Results:\n");
  printf("%s\n%s",buffer,buffer2);

  printf("\n\n----- test_lseek() end -----\n");
  fflush(stdout);
}

int main(int argc, char **argv){
  int srvhndl;

  int port=0,seek=0;
  char protocol[4]="XXX";
  if(argc<4){
          printf("Usage:\n./testy <address> <protocol> <port>\nwhere:\n<protocol> - TCP or UDP\n<port>     - port number<filename> - file to open\n<number>   - length to seek\n<whence>   - seek set:\n             SEEK_CUR: from current position\n             SEEK_SET: from begin\n             SEEK_END: from end\n");
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

  seek=strtol(argv[5], NULL, 10);
  fs_openserver("127.0.0.1", protocol, port, &srvhndl);
  test_lseek(srvhndl,argv[4],seek,argv[6]);
  return 0;
}
