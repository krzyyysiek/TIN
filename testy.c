#include "fs_lib.h"

int main(int argc, char **argv){
  int srvhndl, fd;
  if(fs_openserver("127.0.0.1", "TCP", 9001, &srvhndl) != 0){
    return -1;
  }
  if(fs_open(srvhndl, "test.txt", &fd) != 0){
    return -1;
  }
 
  return 0;
}
