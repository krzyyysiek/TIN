#!/usr/bin/bash
./test_open 127.0.0.1 TCP 9001 testm.txt
./test_read 127.0.0.1 TCP 9001 testm.txt 5
./test_write 127.0.0.1 TCP 9001 testm.txt blablabla
./test_lseek 127.0.0.1 TCP 9001 testm.txt 3 SEEK_SET
./test_fstat 127.0.0.1 TCP 9001 testm.txt
