#include "unp.h"
#include "codes.h"

int write_test(int sockfd){
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

int main( int argc, char **argv){
  int 			sockfd;
  struct sockaddr_in	servaddr;
  
  if (argc != 2) 
    err_quit("usage: tcpli <IPaddress>");

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

  write_test(sockfd);  

  exit(0);
}
