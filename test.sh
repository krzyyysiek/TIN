#!/bin/bash
CONFIG="192.168.0.18 TCP 9000"
./test_open $CONFIG testm.txt
./test_read $CONFIG testm.txt 5
./test_write $CONFIG testm.txt blablabla
./test_lseek $CONFIG testm.txt 3 SEEK_SET
./test_fstat $CONFIG testm.txt
