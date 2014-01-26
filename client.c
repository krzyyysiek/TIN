#include "unp.h"
#include "codes.h"

int fs_openserver(char * ip, char protocol[4], int port ) {
  int 			sockfd;
  struct sockaddr_in	servaddr;
  
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  Inet_pton(AF_INET, ip, &servaddr.sin_addr);
  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
  return sockfd;
}

int fs_open(int srvhndl, char * path) {
  char buffer[5];
  int fd;
  int path_len = strlen(path);
  char msg[2];
  msg[0] = (char)OPEN_FILE;
  msg[1] = (char)path_len; //sciezka niedluzsza niz 255 znakow

  write(srvhndl, &msg, 2);
  read(srvhndl, buffer, 5);
  if (buffer[0] != (char)OPEN_FILE_OK) {
    printf("Couldn't open file on server.");
    return 0; // jakis kod bledu zamiast 0
  } 
  memcpy(&fd, &buffer[1], sizeof(int));

  printf("Received fd: %d\n", fd);
  fflush(stdout);
  return fd;
}

int sock_write_int(int sockfd, int *in_val){
  char buffer[4];
  memcpy(&buffer[0], in_val, 4); 
  write(sockfd, &buffer[0], 4);
}

int fs_write(int srvhndl, int fd, char* data, int len) {
  char code = (char)WRITE;
  char write_msg[9];
  write(srvhndl,&code, 1);
  sock_write_int(srvhndl, &fd);
  sock_write_int(srvhndl, &len); 
  write(srvhndl, data, len);
}


int fs_close(int srvhndl, int fd) {
  char write_msg[5];
  write_msg[0] = (char)CLOSE_FILE;
  memcpy(&write_msg[1], &fd, sizeof(int));
  write(srvhndl, &write_msg, 5);
} 

int fs_read(int srvhndl, int fd, char *buffer, int len){
  char code = (char) READ;
  write(srvhndl, &code, 1);
  sock_write_int(srvhndl, &fd);
  sock_write_int(srvhndl, &len);
}

int main( int argc, char **argv){
  int srvhndl;
  int fd;

  int port=80;
  char protocol[4]="TCP";

  if(argc<4){
	  printf("Usage:\n./server <address> <protocol> <port>\nwhere:\n<protocol> - TCP or UDP\n<port>     - port number\n");
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

  char test[9] = "Testydwaa";
  printf("%s", test);
  
//  if (argc != 2)
//    err_quit("usage: tcpli <IPaddress>");

  srvhndl = fs_openserver(argv[1],protocol,port);
  fd = fs_open(srvhndl, "wat.txt");
  fs_write(srvhndl, fd, test, 8);

  exit(0);
}
