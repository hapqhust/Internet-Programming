all: tcp_server.c tcp_client.c
	gcc -o server tcp_server.c account.c
	gcc -o client tcp_client.c
clean: 
	$(RM) server client *.o *~ 