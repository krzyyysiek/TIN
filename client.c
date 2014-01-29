#include "unp.h"
#include "codes.h"
#define DEBUG 0

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

int fs_openserver(char * ip, char protocol[4], int port, int *srvhndl_out ) {
  int 			sockfd;
  struct sockaddr_in	servaddr;
  
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  Inet_pton(AF_INET, ip, &servaddr.sin_addr);
  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
  *srvhndl_out = sockfd;
  return 0;
}

int fs_closeserver(int srvhndl) {
  close(srvhndl);
}

int fs_open(int srvhndl, char * path, int flags, int *fd_out) {
  char code;
  int fd;
  int path_len = strlen(path);
  char msg[2];
  msg[0] = (char)OPEN_FILE;
  msg[1] = (char)path_len; //sciezka niedluzsza niz 255 znakow

  write(srvhndl, &msg, 2);
  sock_write_int(srvhndl, &flags);
  write(srvhndl, path, path_len);

  read(srvhndl, &code, 1);
  if (code != (char)OPEN_FILE_OK) {
    if(DEBUG){
    	printf("Couldn't open file on server.");
    	fflush(stdout);
    }
    return 0; // jakis kod bledu zamiast 0
  } 
  sock_read_int(srvhndl, &fd);

  if(DEBUG){
	  printf("Received fd: %d\n", fd);
	  fflush(stdout);
  }
  *fd_out = fd;
  return 0;
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
  char code = (char)CLOSE_FILE;
  write(srvhndl, &code, 1);
  sock_write_int(srvhndl, &fd);
} 

int fs_read_out(int sockfd, int fd, int len){
  char code = (char) READ;
  write(sockfd, &code, 1);
  sock_write_int(sockfd, &fd);
  sock_write_int(sockfd, &len);
}
 
int fs_read_in(int sockfd, char *ptr, int len){
  char code;
  char buffer[1024];
  int remaining, to_read, n, offset;

  read(sockfd, &code, 1);  
  if(code != READ_OK){
	if(DEBUG){
		printf("Problem with reading");
		fflush(stdout);
	}
    return -1;
  }
  if(DEBUG){
	  printf("Received READ_OK\n");
  	  fflush(stdout);
  }

  offset = 0;
  remaining = len;
  to_read = min(1024, remaining);
  while(remaining > 0 && (n = read(sockfd, &buffer, to_read)) != 0){
	if(DEBUG){
		printf("Read %d bytes and writing it.\n", n);
    	fflush(stdout);
	}
    memcpy(&ptr[offset], buffer, n);
    remaining -= n;
    offset += n;
  }
}

off_t fs_lseek(int srvhndl, int fd, off_t offset, int whence){
  int off_t_size = sizeof(off_t);
  char buffer[9+off_t_size];
  char code = (char) LSEEK;
  off_t lseek_offset;

  memcpy(&buffer[0], &code, 1);
  memcpy(&buffer[1], &fd, 4);
  memcpy(&buffer[5], &offset, off_t_size);
  memcpy(&buffer[5+off_t_size], &whence, 4);

  write(srvhndl, &code, 1);
  sock_write_int(srvhndl, &fd);
  write(srvhndl, &offset, off_t_size);

  read(srvhndl, &code, 1);
  if(code != LSEEK_OFFSET){
    return -1;
  }else{
    read(srvhndl, &lseek_offset, off_t_size);
    return lseek_offset;
  }
}

int fs_fstat(int srvhndl, int fd, struct stat *buf){
	  int stat_size = sizeof(stat_size);
	  char code = (char) FSTAT;
	  int fstat_return;
	  int time_t_size = sizeof(time_t_size);
	  int off_t_size = sizeof(off_t_size);

	  write(srvhndl, &code, 1);
	  sock_write_int(srvhndl, &fd);

	  read(srvhndl, &code, 1);
	  if(code != FSTAT_STAT){
		  return -1;
	  }else{
		  read(srvhndl, &fstat_return, 4);
		  if(fstat_return<0) return fstat_return;

		  read(srvhndl, &(buf->st_size), off_t_size);
		  read(srvhndl, &(buf->st_atime), time_t_size);
		  read(srvhndl, &(buf->st_mtime), time_t_size);
		  read(srvhndl, &(buf->st_mtime), time_t_size);
		  return 0;
	  }
}

int fs_read(int srvhndl, int fd, char *ptr, int len){

  fs_read_out(srvhndl, fd, len);
  fs_read_in(srvhndl, ptr, len);
}
