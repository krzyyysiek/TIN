server: server.c libunp.a
	gcc server.c libunp.a -o server

client: client.c libunp.a
	gcc client.c libunp.a -o client
