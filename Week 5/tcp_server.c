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
#define SUCCESS_LOGIN "Login is successful!"
#define BLOCK_OR_UNACTIVE_ACC "Account is blocked or inactive"
#define WRONG_PASS "Password is not correct. Please try again!"
#define BLOCK_ACC "Account is blocked!"
#define SIGNOUT "Bye"

struct account
{
	char username[MAX_CHAR];
	char password[MAX_CHAR];
	int status;
	int isLogin;
};

struct account listAccount[10];
int count = 0; // Number account
int numLogin = 0;

void readFile(char *filename)
{
	char user[MAX_CHAR];
	char pass[MAX_CHAR];
	int status;
	FILE *inp = fopen(filename, "r");

	if (!inp)
	{
		printf("Error!! Can not read file\n");
		exit(0);
	}
	else
	{
		do
		{
			if (fscanf(inp, "%s %s %d", user, pass, &status) > 0)
			{
				strcpy(listAccount[count].username, user);
				strcpy(listAccount[count].password, pass);
				listAccount[count].status = status;
				listAccount[count].isLogin = FALSE;
				count++;
			}
			else
				break;
		} while (TRUE);
	}
}

int getAccountStatus(char *account)
{
	for (int i = 0; i < count; i++)
	{
		if (strcmp(account, listAccount[i].username) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

int getUserStatus(char *account, char *password)
{
	for (int i = 0; i < count; i++)
	{
		if (strcmp(account, listAccount[i].username) == 0 && strcmp(password, listAccount[i].password) == 0)
		{
			return listAccount[i].status;
		}
	}
}

void setUserStatus(char *account, char *password, int value)
{
	for (int i = 0; i < count; i++)
	{
		if (strcmp(account, listAccount[i].username) == 0 && strcmp(password, listAccount[i].password) == 0)
		{
			listAccount[i].status = value;
		}
	}
}

int getPasswordStatus(char *password)
{
	for (int i = 0; i < count; i++)
	{
		if (strcmp(password, listAccount[i].password) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void setLoginStatus(char *account, char *password, int status)
{
	for (int i = 0; i < count; i++)
	{
		if (strcmp(account, listAccount[i].username) == 0 && strcmp(password, listAccount[i].password) == 0)
		{
			listAccount[i].isLogin = status;
		}
	}
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

int main(int argc, char *argv[])
{
	int server_port;			/* Port that will be opened */
	int listen_sock, conn_sock; /* file descriptors */
	char account[BUFF_SIZE];
	char password[BUFF_SIZE];
	char recv_data[BUFF_SIZE];		// temp
	int bytes_sent, bytes_received; // temp
	struct sockaddr_in server;		/* server's address information */
	struct sockaddr_in client;		/* client's address information */
	int sin_size;
	int bytes_received_account, bytes_received_password;

	// Step 0: Get data from command
	if (argc != 2)
	{
		printf("Wrong command!\n");
		exit(0);
	}
	else
	{
		server_port = atoi(argv[1]);
		readFile("account.txt");
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
	while (1)
	{
		// accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
			perror("\nError: ");

		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

		// start conversation
		while (TRUE)
		{
			// receives message from client
			bytes_received_account = recv(conn_sock, account, BUFF_SIZE - 1, 0);   // blocking
			bytes_received_password = recv(conn_sock, password, BUFF_SIZE - 1, 0); // blocking
			if (strcmp(account, SIGNOUT) == 0) {
				sendMessage(SIGNOUT, conn_sock);
				setLoginStatus(account, password, FALSE);
			}
			if (bytes_received_account <= 0 || bytes_received_password <= 0)
			{
				printf("\nConnection closed");
				break;
			}
			else
			{
				account[bytes_received_account] = '\0';
				password[bytes_received_password] = '\0';
				account[strcspn(account, "\n")] = 0;
				password[strcspn(password, "\n")] = 0;
				printf("\nReceive: %s %s \n", account, password);
			}
			// start IPO process
			if (getAccountStatus(account) == TRUE && getPasswordStatus(password) == TRUE)
			{
				if (getUserStatus(account, password) == TRUE)
				{
					sendMessage(SUCCESS_LOGIN, conn_sock);
					setLoginStatus(account, password, TRUE);
					numLogin = 0;
				}
				else
				{
					sendMessage(BLOCK_OR_UNACTIVE_ACC, conn_sock);
				}
			}
			else if (getAccountStatus(account) == FALSE || getPasswordStatus(password) == FALSE)
			{
				if (numLogin == 2)
				{
					sendMessage(BLOCK_ACC, conn_sock);
					setUserStatus(account, password, FALSE);
				}
				else
				{
					sendMessage(WRONG_PASS, conn_sock);
					numLogin++;
				}
			}
		} // end conversation
		close(conn_sock);
	}

	close(listen_sock);
	return 0;
}