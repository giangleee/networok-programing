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

// defind message
#define SUCCESS_LOGIN "Login is successful!"
#define BLOCK_OR_UNACTIVE_ACC "Account is blocked or inactive"
#define WRONG_PASS "Password is not correct. Please try again!"
#define BLOCK_ACC "Account is blocked!"
#define SIGNOUT "Bye"

// defind message code
#define SUCCESS_LOGIN_CODE 1
#define BLOCK_OR_UNACTIVE_ACC_CODE -3
#define WRONG_PASS_CODE -2
#define BLOCK_ACC_CODE -1
#define SIGNOUT_CODE 0

int getInput(char *string)
{
	memset(string, '\0', (strlen(string) + 1));
	fgets(string, BUFF_SIZE, stdin);
	return strlen(string);
}

int main(int argc, char *argv[])
{

	// IP and PORT detail
	int server_port;
	char server_ip[MAX_CHAR];
	int client_sock;
	char account[BUFF_SIZE];
	char password[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len_account, msg_len_password;
	int bytes_sent_account, bytes_sent_password;
	int bytes_received_account, bytes_received_password;

	// Step 0: Get data from command
	if (argc != 3)
	{
		printf("Wrong command!\n");
		exit(0);
	}
	else
	{
		server_port = atoi(argv[2]);
		strcpy(server_ip, argv[1]);
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

	// Step 4: Communicate with server
	while (1)
	{
		// get account and password
		msg_len_account = getInput(account);
		if (msg_len_account == 0)
			break;
		char checkInput[MAX_CHAR];
		strcpy(checkInput, account);
		checkInput[strcspn(checkInput, "\n")] = 0;
		if (strcmp(SIGNOUT, checkInput) == 0)
		{
			bytes_sent_account = send(client_sock, checkInput, msg_len_account, 0);
			bytes_sent_account = send(client_sock, checkInput, msg_len_account, 0);
		}
		else
		{
			printf("\nPlease enter the login password:");
			msg_len_password = getInput(password);
			if (msg_len_password == 0)
				break;

			// send message
			bytes_sent_account = send(client_sock, account, msg_len_account, 0);
			bytes_sent_password = send(client_sock, password, msg_len_password, 0);
		}

		if (bytes_sent_account <= 0)
		{
			printf("\nConnection closed!\n");
			break;
		}

		// receive echo reply
		int code, bytes_received;
		char message[BUFF_SIZE];
		bytes_received = recv(client_sock, message, BUFF_SIZE - 1, 0);
		if (bytes_received <= 0)
		{
			printf("\nError!Cannot receive data from sever!\n");
			break;
		}
		printf("%s\n", message);
		// account[bytes_received_account] = '\0';
	}

	// Step 4: Close socket
	close(client_sock);
	return 0;
}