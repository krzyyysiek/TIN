#include "unp.h"
#include "codes.h"

int sock_read_int(int sockfd, int *value_out){
  char buffer[4];
  read(sockfd, &buffer, 4);
  memcpy(value_out, &buffer, 4);
}

int handle_write(int sockfd){
  char fd_buffer[4];
  char data_len_buffer[4];

  FILE* fd;
  int data_len;

  char data_buffer[1024];
  int remaining;

  int n;
  int to_read;

  read(sockfd, &fd_buffer, 4);
  read(sockfd, &data_len_buffer, 4);

  memcpy(&fd, fd_buffer, sizeof(int));
  memcpy(&data_len, data_len_buffer, sizeof(int));

  remaining= data_len;
  to_read = min(1024, remaining);
  while( remaining > 0 && (n = read(sockfd, &data_buffer, to_read)) != 0){
    printf("Writing %d bytes to file", n);
    fwrite(&data_buffer, 1, n, fd);
 //   fwrite(fd, &data_buffer, n); 
    remaining -= n;
    to_read = min(1024, remaining);
  }

  printf("File write, fd: %d, len: %d\n", fd, data_len);
  fflush(stdout);
}

int handle_open_file(sockfd){
  FILE* fd = NULL;
  char msg_buffer[5];
  char len;

  msg_buffer[0] = (char)OPEN_FILE_OK;

  read(sockfd, &len, 1);
  printf("File path len: %d\n", (int)len);

  fd = fopen("./test.txt", "w+");
  if(fd == NULL){
    printf("Problem?");
  }
  memcpy(&msg_buffer[1], &fd, sizeof(int));

  write(sockfd, &msg_buffer, 5); 
  fflush(stdout);
}	

int handle_close(int sockfd){
  char fd_buffer[4];
  FILE* fd = NULL;
  read(sockfd, &fd_buffer, 4);
  memcpy(&fd, fd_buffer, sizeof(int));

  if(fclose(fd) == EOF){
    printf("Problem?");
  }
  printf("File closed\n");
}

int handle_read_in(int sockfd, int *fd, int *len){
  sock_read_int(sockfd, fd);
  sock_read_int(sockfd, len);

  printf("File read, fd: %d, len: %d\n", *fd, *len);
  fflush(stdout);
}

int handle_read_out(int sockfd, int *fd, int *data_len){
  // TODO
  printf("Dummy reading file");
  fflush(stdout);
}

int handle_read(sockfd){
  int fd;
  int data_len;
  handle_read_in(sockfd, &fd, &data_len);
  handle_read_out(sockfd, &fd, &data_len);
}

int handle_client(int sockfd){
  ssize_t	n;
  char		code;

  while( (n = read(sockfd, &code, 1)) != 0){
    printf("Received frame with code: %d\n", (int)code);
    fflush(stdout);
    switch(code){
      case OPEN_FILE:
        handle_open_file(sockfd);
        break;
      case WRITE:
    	printf ("a");
    	fflush(stdout);
        handle_write(sockfd);
        break;
      case CLOSE_FILE:
        handle_close(sockfd);
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
