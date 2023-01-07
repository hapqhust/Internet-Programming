all: server-socket.c client-socket.c
	gcc -pthread -o server server-socket.c
	gcc -pthread -o client client-socket.c
clean: 
	$(RM) server client *.o *~ 