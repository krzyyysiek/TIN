#include "fs_lib.h"
#include "stdio.h"
#include "fcntl.h"

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

  return 0;
}
