#include "unp.h"
#include "codes.h"

#define sDEBUG printf("wat_server\n");fflush(stdout);

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
  sDEBUG

  memcpy(&fd, fd_buffer, sizeof(int));
  memcpy(&data_len, data_len_buffer, sizeof(int));
  sDEBUG

  remaining= data_len;
  to_read = min(1024, remaining);
  sDEBUG
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
  char code;
  char len;
  char fd_msg[4];
  char path[255];

  read(sockfd, &len, 1);
  printf("File path len: %d\n", (int)len); fflush(stdout); 
  read(sockfd, &path, (int)len);
  path[(int)len]='\0'; 
  // TODO ewentualne tlumaczenie podanej sciezki na sciezke w serwerze 
  fd = fopen(path, "wr+");
  if(fd == NULL){
    printf("Problem opening file");
    code = (char)CANT_OPEN_FILE;
  }
 
  code = (char)OPEN_FILE_OK;
  write(sockfd, &code, 1);  

  memcpy(&fd_msg[0], &fd, sizeof(int));
  write(sockfd, &fd_msg, 4); 
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

int handle_read_out(int sockfd, int *ptr_fd, int *ptr_len){
  char code = (char)READ_OK;
  char buffer[1024];
  int remaining, len, n, to_read;
  FILE* fd;

  sDEBUG

  fd = *ptr_fd;
  len = *ptr_len;

  printf("fd 2 : %d", fd); fflush(stdout);
  
  write(sockfd, &code, 1);
  //printf("wat"); fflush(stdout);

  remaining = len;
  to_read = min(1024, remaining);
  sDEBUG
  while( remaining > 0 && (n = fread(buffer, 1, to_read, fd)) != 0){
    sDEBUG
    printf("Read %d bytes and writing to socket", n);
    fflush(stdout);
    write(sockfd, buffer, n);
    remaining -= n;
    to_read = min(1024, remaining);
  }
  fflush(stdout);
    printf("wasdao");
    fflush(stdout);
}

int handle_read(sockfd){
  int fd;
  int data_len;
  handle_read_in(sockfd, &fd, &data_len);
  handle_read_out(sockfd, &fd, &data_len);
}

int handle_lseek(sockfd){
  char code;
  off_t offset,lseek_ofsset;
  int whence,fd;

  read(sockfd, &fd, sizeof(int));
  read(sockfd, &offset, sizeof(off_t));
  read(sockfd, &whence, sizeof(int));
  lseek_ofsset=lseek(fd, offset, whence);

  code=LSEEK_OFFSET;
  write(sockfd, &code, 1);
  write(sockfd, &lseek_ofsset, sizeof(off_t));
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
      case LSEEK:
        handle_lseek(sockfd);
        break;
      case CLOSE_FILE:
        handle_close(sockfd);
        break;
      case READ:
        handle_read(sockfd);
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
  int port=80;
  char protocol[4]="TCP";

  if(argc<3){
	  printf("Usage:\n./server <protocol> <port>\nwhere:\n<protocol> - TCP or UDP\n<port>     - port number\n");
	  return 0;
  }else{
  	  memcpy(&protocol, argv[1], 4);
  	  port=strtol(argv[2], NULL, 10);
  	  if(strcmp(protocol,"TCP")!=0 && strcmp(protocol,"UDP")!=0){
  		  printf("Wrong protocol specified.\n");
  		  return 0;
  	  }
  	  printf("Running with %s connection on port %d...\n",protocol,port);

  }
  
  if(strcmp(protocol,"TCP")==0){
	  listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  }
  else if(strcmp(protocol,"UDP")==0){
	  return 0; // poki UDP nie dziala
	  listenfd = Socket(AF_INET, SOCK_DGRAM, 0);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family 		= AF_INET;
  servaddr.sin_addr.s_addr 	= htonl(INADDR_ANY);
  servaddr.sin_port 		= htons(port); //htons(SERV_PORT);
  
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
