#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	int client_sock;
	char buff[BUFF_SIZE + 1];
	struct sockaddr_in server_addr_info; /* server's address information */
	int bytes_sent, bytes_received;
	char docs_path[BUFF_SIZE + 1], server_addr[BUFF_SIZE + 1];
	int server_port;

	// Step 0: Get data from command
	if (argc != 4)
	{
		printf("Wrong command!\n");
		exit(0);
	}
	else
	{
		strcpy(server_addr, argv[1]);
		server_port = atoi(argv[2]);
		strcpy(docs_path, argv[3]);
	}

	// Step 1: Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Step 2: Specify server address
	server_addr_info.sin_family = AF_INET;
	server_addr_info.sin_port = htons(server_port);
	server_addr_info.sin_addr.s_addr = inet_addr(server_addr);

	// Step 3: Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr_info, sizeof(struct sockaddr)) < 0)
	{
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

	// Step 4: Communicate with server
	// send file text path
	bytes_sent = send(client_sock, docs_path, strlen(docs_path), 0);
	if (bytes_sent < 0)
		perror("\nError: ");

	// receive echo reply
	bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.\n");

	buff[bytes_received] = '\0';
	printf("New file's path: %s\n", buff);

	// receive echo reply
	bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.\n");

	buff[bytes_received] = '\0';
	printf("Number of bytes: %s\n", buff);

	// Step 4: Close socket
	close(client_sock);
	return 0;
}
