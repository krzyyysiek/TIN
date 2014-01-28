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

int fs_closeserver(int srvhndl) {
  close(srvhndl);
}

int fs_open(int srvhndl, char * path) {
  char buffer[5];
  int fd;
  int path_len = strlen(path);
  char msg[2];
  msg[0] = (char)OPEN_FILE;
  msg[1] = (char)path_len; //sciezka niedluzsza niz 255 znakow

  write(srvhndl, &msg, 2);
  write(srvhndl, path, path_len);

  read(srvhndl, buffer, 5);
  if (buffer[0] != (char)OPEN_FILE_OK) {
    printf("Couldn't open file on server.");
    fflush(stdout);
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
    printf("Problem with reading");
    fflush(stdout);
    return -1;
  }
  printf("Received READ_OK\n");
  fflush(stdout);

  offset = 0;
  remaining = len;
  to_read = min(1024, remaining);

  while(remaining > 0 && (n = read(sockfd, &buffer, to_read)) != 0){
    printf("Read %d bytes and writing it.\n", n);
    fflush(stdout);

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

int fs_read(int srvhndl, int fd, char *ptr, int len){
  fs_read_out(srvhndl, fd, len);
  fs_read_in(srvhndl, ptr, len);
}

int test_read() {
  int srvhndl, fd, i;
  char buffer[1024];

  srvhndl = fs_openserver("127.0.0.1", "TCP", 9001);
  fd = fs_open(srvhndl, "test.txt");

  fs_read(srvhndl, fd, buffer,5); 

  for(i=0; i<5; i++){
    printf("%c", buffer[i]);
  }
  printf("\n");
  fflush(stdout);
}

int main( int argc, char **argv){
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

  test_read();
  exit(0);
}
