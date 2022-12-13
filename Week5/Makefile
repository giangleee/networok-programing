CC = gcc

tcp: tcp_client.o tcp_server.o
	$(CC) tcp_client.o -o client
	$(CC) tcp_server.o -o server

tcp.o: tcp_client.c tcp_server.o
	$(CC) -c tcp_client.c
	$(CC) -c tcp_server.c

clean: 
	$(RM) tcp_client tcp_server server client *.o *~ 