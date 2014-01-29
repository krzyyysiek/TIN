server: server.c libunp.a
	gcc server.c libunp.a -o server
testy: testy.c libunp.a
	gcc testy.c fs_lib.a libunp.a -o testy
fs_lib.a: client.c
	gcc -c client.c libunp.a -o fs_lib.a
test_all: fs_lib.a test_open test_write test_read test_lseek test_fstat 
test_open: test_open.c fs_lib.a
	gcc test_open.c fs_lib.a libunp.a -o test_open
test_read: test_read.c fs_lib.a
	gcc test_read.c fs_lib.a libunp.a -o test_read
test_write: test_write.c fs_lib.a
	gcc test_write.c fs_lib.a libunp.a -o test_write
test_lseek: test_lseek.c fs_lib.a
	gcc test_lseek.c fs_lib.a libunp.a -o test_lseek
test_fstat: test_fstat.c fs_lib.a
	gcc test_fstat.c fs_lib.a libunp.a -o test_fstat