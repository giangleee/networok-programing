#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 8192

#define TRUE 1
#define FALSE 0
#define MAX_CHAR 100

// defind binary code
#define ONE "1"
#define ZERO "0"

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

void exportImage(char *recv_data)
{
	// // Turn binary code into image
	FILE *ptr_txt_in;
	FILE *ptr_out;
	int c = 0;
	unsigned char buf[8];
	int j = 0;

	char fileRootClient[MAX_CHAR];
	strcpy(fileRootClient, "./home/image/");
	strcat(fileRootClient, recv_data);

	ptr_out = fopen(fileRootClient, "wb");
	ptr_txt_in = fopen("image_binary_code.txt", "r");

	if (!ptr_txt_in)
	{
		printf("Unable to open file\n");
		return;
	}

	while ((c = fgetc(ptr_txt_in)) != EOF)
	{
		buf[j++] = c;
		if (j == 8)
		{
			fputc(bytefromtext(buf), ptr_out);
			j = 0;
		}
	}
}

int main(int argc, char *argv[])
{

	// IP and PORT detail
	int server_port;
	char server_ip[MAX_CHAR];
	char client_file[MAX_CHAR];
	int client_sock;
	struct sockaddr_in server_addr; /* server's address information */
	char buff[BUFF_SIZE];
	int msg_len, bytes_sent, bytes_received;

	// Step 0: Get data from command
	if (argc != 4)
	{
		printf("Wrong command!\n");
		exit(0);
	}
	else
	{
		// check file is validate
		server_port = atoi(argv[2]);
		strcpy(server_ip, argv[1]);
		strcpy(client_file, argv[3]);
		if (getFileFormat(client_file) == FALSE)
		{
			printf("Error: Wrong File Format!\n");
			exit(0);
		}
	}

	// Step 1: Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

	// Step 3: Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

	// Store data from server
	FILE *ptr_txt_out;
	ptr_txt_out = fopen("image_binary_code.txt", "w");
	printf("[START GETTING DATA FROM SERVER]\n");

	// while (TRUE)
	// {
	// Step 4: Communicate with server
	client_file[strcspn(client_file, "\n")] = 0;
	msg_len = strlen(client_file);
	if (msg_len != 0)
	{
		bytes_sent = send(client_sock, client_file, msg_len, 0);
		if (bytes_sent <= 0)
		{
			printf("\nConnection closed!\n");
		}
	}

	while (TRUE)
	{
		// receive echo reply
		bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
		if (bytes_received <= 0)
		{
			printf("\nError!Cannot receive data from sever!\n");
		}

		buff[bytes_received] = '\0';
		printf("%s", buff);
		// buff[strcspn(buff, "\n")] = 0;

		// if (strcmp(buff, "0") == 0)
		// {
		// 	// printf("%s", buff);
		// 	fputc('0', ptr_txt_out);
		// }
		// else if (strcmp(buff, "1") == 0)
		// {
		// 	// printf("%s", buff);
		// 	fputc('1', ptr_txt_out);
		// }
		// printf("%s", buff);
	}

	printf("Start export image\n");
	exportImage(client_file);
	// }

	// Step 5: Close socket
	close(client_sock);
	close(ptr_txt_out);
	return 0;
}