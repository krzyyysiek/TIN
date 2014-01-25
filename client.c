#include "unp.h"
#include "codes.h"

int test_write_out(int sockfd, int filefd){

}

int test_open_out(int sockfd){
  int fd = 11111;
  int data_len = 22222;
  char test_msg[2];
  test_msg[0] = (char)OPEN_FILE;
  test_msg[1] = (char)15;

  write(sockfd, &test_msg, 2);

  char write_msg[9];
  write_msg[0] = (char)WRITE;
  memcpy(&write_msg[1], &fd, sizeof(int));
  //write_msg[1] = (int)11111;
  memcpy(&write_msg[5], &data_len, sizeof(int));
  //write_msg[5] = (int)22222;

  write(sockfd, &write_msg, 9);
}

int test_open_in(int sockfd){
  int fd;
  char buffer[5];
  read(sockfd, buffer, 5);

  memcpy(&fd, &buffer[1], sizeof(int));

  printf("Received fd: %d\n", fd);
  fflush(stdout);
  return fd;
}

int main( int argc, char **argv){
  int 			sockfd;
  struct sockaddr_in	servaddr;
  int fd;
  
  if (argc != 2) 
    err_quit("usage: tcpli <IPaddress>");

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

  test_open_out(sockfd);  
  fd = test_open_in(sockfd);

  test_write_out(sockfd, fd);

  exit(0);
}
