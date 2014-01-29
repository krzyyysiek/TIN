#include "unp.h"
#include "codes.h"
#include "errno.h"

int sock_write_int(int sockfd, int *in_val){
  char buffer[4];
  memcpy(&buffer[0], in_val, 4);
  write(sockfd, &buffer[0], 4);
}

int sock_read_int(int sockfd, int *value_out){
  char buffer[4];
  read(sockfd, &buffer, 4);
  memcpy(value_out, &buffer, 4);
}

int handle_write(int sockfd){
  char fd_buffer[4];
  char data_len_buffer[4];

  int fd;
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
    write(fd, &data_buffer, n);
    //fwrite(&data_buffer, 1, n, fd);
    remaining -= n;
    to_read = min(1024, remaining);
  }

  printf("File write, fd: %d, len: %d\n", fd, data_len);
  fflush(stdout);
}

int handle_open_file(sockfd){
  int fd = NULL;
  char code, len, fd_msg[4], path[255];
  int flags;

  read(sockfd, &len, 1);
  sock_read_int(sockfd, &flags);
  printf("File path len: %d, flags:%d\n", (int)len, flags); fflush(stdout);

  read(sockfd, &path, (int)len);
  path[(int)len]='\0'; 
  fd = open(path, flags);
  if(fd < 0){
    printf("Problem opening file");
    fflush(stdout);
    code = (char)CANT_OPEN_FILE;
  }
 
  code = (char)OPEN_FILE_OK;
  write(sockfd, &code, 1);  

  printf("Opened file fd: %d\n",fd);
  fflush(stdout);

  sock_write_int(sockfd, &fd);
}	

int handle_close(int sockfd){
  int fd = NULL;
  sock_read_int(sockfd, &fd);

  printf("Close file fd: %d\n",fd);
  fflush(stdout);
  if(close(fd) == -1){
    printf("Problem with closing. Errno: %d\n",errno);
  }
  fflush(stdout);
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
  int fd, remaining, len, n, to_read;

  fd = *ptr_fd;
  len = *ptr_len;
  
  write(sockfd, &code, 1);

  remaining = len;
  to_read = min(1024, remaining);
  while( remaining > 0 && (n = read(fd, buffer, to_read)) != 0){
    if(n == -1){
    	printf("Read error, code: %d", errno);
    	fflush(stdout);
    	return -1;
    }
    printf("Read %d bytes and writing to socket", n);
    fflush(stdout);
    write(sockfd, buffer, n);
    remaining -= n;
    to_read = min(1024, remaining);
  }
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
  int off_t_size = sizeof(off_t_size);
  char buffer[off_t_size];

  read(sockfd, &fd, sizeof(int));
  read(sockfd, &offset, off_t_size);
  read(sockfd, &whence, sizeof(int));
  lseek_ofsset=lseek(fd, offset, whence);

  code=LSEEK_OFFSET;
  write(sockfd, &code, 1);
  memcpy(&buffer, &lseek_ofsset, off_t_size);
  write(sockfd, &buffer, off_t_size);
  printf("a"); fflush(stdout);
}

int handle_fstat(sockfd){
  int fd;
  int stat_size = sizeof(stat_size);
  int time_t_size = sizeof(time_t_size);
  int off_t_size = sizeof(off_t_size);
  char code = (char) FSTAT;
  int fstat_return;
  struct stat buf;

  read(sockfd, &fd, 4);
  fstat_return=fstat(fd, &buf);

  code=LSEEK_OFFSET;
  write(sockfd, &code, 1);
  write(sockfd, &buf.st_size, off_t_size);
  write(sockfd, &buf.st_atime, time_t_size);
  write(sockfd, &buf.st_mtime, time_t_size);
  write(sockfd, &buf.st_ctime, time_t_size);
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
      case FSTAT:
        handle_fstat(sockfd);
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

  int port=0;
  char protocol[4]="XXX";
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
