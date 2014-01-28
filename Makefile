server: server.c libunp.a
	gcc server.c libunp.a -o server
testy: testy.c fs_lib.a libunp.a
	gcc testy.c fs_lib.a libunp.a -o testy
fs_lib.a: client.c libunp.a
	gcc -c client.c libunp.a -o fs_lib.a
