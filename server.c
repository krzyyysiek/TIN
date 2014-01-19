#include "unp.h"
#include "codes.h"


int handle_client_write(sockfd){
  char fd_buffer[4];
  char data_len_buffer[4];

  int fd;
  int data_len;

  read(sockfd, &fd_buffer, 4);
  read(sockfd, &data_len_buffer, 4);

  memcpy(&fd, fd_buffer, sizeof(int));
  memcpy(&data_len, data_len_buffer, sizeof(int));
  //fd = *fd_buffer;
  //data_len = *data_len_buffer;

  printf("File write, fd: %d, len: %d\n", fd, data_len);
  fflush(stdout);
}

int handle_client_open_file(sockfd){
  char len;

  read(sockfd, &len, 1);
  printf("File path len: %d\n", (int)len);
  fflush(stdout);
}

int handle_client_test(){
  printf("test");
  fflush(stdout);
}

int handle_client(int sockfd){
  ssize_t	n;

  char		code;

  while( (n = read(sockfd, &code, 1)) != 0){
    printf("Received frame with code: %d\n", (int)code);
    fflush(stdout); 
    switch(code){
      case TEST:
	handle_client_test();
        break;
      case OPEN_FILE:
        handle_client_open_file(sockfd);
        break;
      case WRITE:
        handle_client_write(sockfd);
        break;
    }
  }  
}

int main(int argc, char **argv)
{
  int listenfd, connfd;
  pid_t 		childpid;
  socklen_t		clilen;
  struct sockaddr_in	cliaddr, servaddr;
  
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family 		= AF_INET;
  servaddr.sin_addr.s_addr 	= htonl(INADDR_ANY);
  servaddr.sin_port 		= htons(SERV_PORT);
  
  Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
  
  Listen(listenfd, LISTENQ);

  while(1){
    clilen = sizeof(cliaddr);
    connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

    if( (childpid = Fork()) == 0) {
      Close(listenfd);
      handle_client(connfd);
      exit(0);
    }
    Close(connfd);
  }
}
