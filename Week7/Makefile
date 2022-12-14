CC = gcc
VAL = valgrind

tcp: tcp_client.o tcp_server.o
	$(CC) tcp_client.o -o client
	$(CC) tcp_server.o -o server

tcp.o: tcp_client.c tcp_server.o
	$(CC) -c tcp_client.c
	$(CC) -c tcp_server.c

clean: 
	$(RM) tcp_client tcp_server server client *.o *~ 

valgrind:
	$(CC) -o ${where} -std=c11 -Wall -ggdb3 tcp_$(where).c
	$(VAL) --tool=memcheck --leak-check=full --show-leak-kinds=all ./${where}

