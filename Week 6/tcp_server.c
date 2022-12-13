#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BACKLOG 2 /* Number of allowed connections */
#define BUFF_SIZE 1024

#define TRUE 1
#define FALSE 0
#define MAX_CHAR 100

// defind message
#define FILE_TRANSFER_COMPLETED "File Transfer Is Completed..."
#define FILE_NOT_FOUND "Error: File Not Found!"
#define FILE_EXIT "File Already Exists"

// defind code
#define FILE_TRANSFER_COMPLETED_CODE 1
#define FILE_NOT_FOUND_CODE 2
#define FILE_EXIT_CODE 3

// defind binary code
#define ONE "1"
#define ZERO "0"

int status;
char pathMain[MAX_CHAR];

char bytefromtext(unsigned char *text)
{
	char result = 0;
	for (int i = 0; i < 8; i++)
	{
		if (text[i] == '1')
		{
			result |= (1 << (7 - i));
		}
	}
	return result;
}

void sendMessage(char *message, int conn_sock)
{
	int bytes_sent;
	bytes_sent = send(conn_sock, message, strlen(message), 0); /* send to the client welcome message */
	if (bytes_sent <= 0)
	{
		printf("\nConnection closed");
	}
}

void checkDabatase(char *filename, char *filecheck, int conn_sock)
{
	char fileDownload[MAX_CHAR];
	char path[MAX_CHAR];

	FILE *inp = fopen(filename, "r");

	if (!inp)
	{
		exit(0);
	}
	{
		do
		{
			if (fscanf(inp, "%s %s", fileDownload, path) > 0)
			{
				if (strcmp(fileDownload, filecheck) == 0)
				{
					stpcpy(pathMain, path);
					break;
				}
			}
			else
			{
				sendMessage(FILE_NOT_FOUND, conn_sock);
				status = FILE_NOT_FOUND_CODE;
				break;
			}
		} while (TRUE);
	}
}

int getFileFormat(char *string)
{
	char source[MAX_CHAR];
	strcpy(source, string);
	char *token = strtok(source, ".");
	token = strtok(NULL, ".");
	if ((strcmp(token, "jpg") == 0) || (strcmp(token, "bmp") == 0) || (strcmp(token, "png") == 0))
	{
		return TRUE;
	}
	return FALSE;
}

int main(int argc, char *argv[])
{
	int server_port;				/* Port that will be opened */
	int listen_sock, conn_sock;		/* file descriptors */
	char recv_data[BUFF_SIZE];		// temp
	int bytes_sent, bytes_received; // temp
	struct sockaddr_in server;		/* server's address information */
	struct sockaddr_in client;		/* client's address information */
	int sin_size;

	// Step 0: Get data from command
	if (argc != 2)
	{
		printf("Wrong command!\n");
		exit(0);
	}
	else
	{
		server_port = atoi(argv[1]);
	}

	// Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	// Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(server_port);		/* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */
	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{ /* calls bind() */
		perror("\nError: ");
		return 0;
	}

	// Step 3: Listen request from client
	if (listen(listen_sock, BACKLOG) == -1)
	{ /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	// Step 4: Communicate with client
	while (TRUE)
	{
		// accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
			perror("\nError: ");

		printf("\nYou got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

		// start conversation
		while (TRUE)
		{
			// receives message from client
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0); // blocking
			if (bytes_received <= 0)
			{
				printf("\nConnection closed");
				break;
			}
			else
			{
				recv_data[bytes_received] = '\0';
				printf("\nReceive: %s \n", recv_data);
			}
			checkDabatase("./server_data/database/database.txt", recv_data, conn_sock);

			if (status != FILE_NOT_FOUND_CODE)
			{
				char fileRootClient[MAX_CHAR];
				strcpy(fileRootClient, "./home/image/");
				strcat(fileRootClient, recv_data);
				if (access(fileRootClient, F_OK) == 0)
				{
					sendMessage(FILE_EXIT, conn_sock);
				}
				else
				{
					char mainPath[MAX_CHAR];
					strcpy(mainPath, ".");
					strcat(mainPath, pathMain);

					// // Turn image into binary code
					FILE *ptr_in;
					FILE *ptr_txt_out;
					int c = 0;
					unsigned char buf[8];
					int j = 0;

					ptr_in = fopen(mainPath, "rb");
					ptr_txt_out = fopen("image_binary_code.txt", "w");

					if (!ptr_in)
					{
						// echo to client
						sendMessage(FILE_NOT_FOUND, conn_sock);
					}

					while ((c = fgetc(ptr_in)) != EOF)
					{
						for (int i = 0; i <= 7; i++)
						{
							if (c & (1 << (7 - i)))
							{
								// sendMessage("1", conn_sock);
								fputc('1', ptr_txt_out);
							}
							else
							{
								// sendMessage("0", conn_sock);
								fputc('0', ptr_txt_out);
								// dataStore = dataStore + '0';
							}
						}
					}

					// // Turn binary code into image
					FILE *ptr_txt_in;
					// FILE *ptr_out;
					// c = 0;

					// char fileRootClient[MAX_CHAR];
					// strcpy(fileRootClient, "./home/image/");
					// strcat(fileRootClient, recv_data);

					// ptr_out = fopen(fileRootClient, "wb");
					ptr_txt_in = fopen("image_binary_code.txt", "r");

					// if (!ptr_txt_in)
					// {
					// 	printf("Unable to open file\n");
					// 	return 1;
					// }

					while ((c = fgetc(ptr_txt_in)) != EOF)
					{
						buf[j++] = c;
						if (j == 8)
						{
							// fputc(bytefromtext(buf), ptr_bmp_out);
							printf("%hhn", buf);
							j = 0;
						}
					}

					// // Done the process
					// fclose(ptr_out);
					fclose(ptr_in);
					fclose(ptr_txt_out);
					// fclose(ptr_txt_in);
					// remove("image_binary_code.txt");

					// echo to client
					sendMessage(FILE_TRANSFER_COMPLETED, conn_sock);
				}
			}

		} // end conversation
		close(conn_sock);
	}

	close(listen_sock);
	return 0;
}