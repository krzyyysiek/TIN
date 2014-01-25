#include "unp.h"
#include "codes.h"

int fs_openserver(char * ip ) {
  int 			sockfd;
  struct sockaddr_in	servaddr;
  
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
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
  // TODO check czy bajt 0. to OK_OPEN
  memcpy(&fd, &buffer[1], sizeof(int));

  printf("Received fd: %d\n", fd);
  fflush(stdout);
  return fd;
}

int fs_write(int srvhndl, int fd, char* data, int len) {
  // TODO wysylanie arbitralnych danych
  char write_msg[13];
  int data_len = 4;
  write_msg[0] = (char)WRITE;
  memcpy(&write_msg[1], &fd, sizeof(int));
  memcpy(&write_msg[5], &data_len, sizeof(int));

  write_msg[9] = 'L';
  write_msg[10] = 'O';
  write_msg[11] = 'L';
  write_msg[12] = 'O';
  write(srvhndl, &write_msg, 13);
}


int main( int argc, char **argv){
  int srvhndl;
  int fd;
  
  if (argc != 2) 
    err_quit("usage: tcpli <IPaddress>");

  srvhndl = fs_openserver(argv[1]);
  fd = fs_open(srvhndl, "wat.txt");
  fs_write(srvhndl, fd, NULL, 0);

  exit(0);
}
