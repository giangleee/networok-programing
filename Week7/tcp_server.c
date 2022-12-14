#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>

#define BACKLOG 20
#define BUFF_SIZE 1024
#define TRUE 1
#define FALSE 0

/* Handler process signal*/
void sig_chld(int signo);

/*
 * Receive and echo message to client
 * [IN] sockfd: socket descriptor that connects to client
 */
void echo(int sockfd);

/**
 * Reverse string in place 
*/
void reverse(char s[]);

/** 
 * Convert n to characters in s 
*/
void itoa(int n, char s[]);

int main(int argc, char *argv[])
{

	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server;	/* server's address information */
	struct sockaddr_in client;	/* client's address information */
	pid_t pid;
	int sin_size;
	int PORT;

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		printf("socket() error\n");
		return 0;
	}

	// Step 0: Get data from command
	if (argc != 2)
	{
		printf("Wrong command!\n");
		exit(0);
	}
	else
	{
		PORT = atoi(argv[1]);
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	if (listen(listen_sock, BACKLOG) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	/* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);

	while (TRUE)
	{
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
		{
			if (errno == EINTR)
				continue;
			else
			{
				perror("\nError: ");
				return 0;
			}
		}

		/* For each client, fork spawns a child, and the child handles the new client */
		pid = fork();

		/* fork() is called in child process */
		if (pid == 0)
		{
			close(listen_sock);
			printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
			echo(conn_sock);
			exit(0);
		}

		/* The parent closes the connected socket since the child handles the new client */
		close(conn_sock);
	}
	close(listen_sock);
	return 0;
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	/* Wait the child process terminate */
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("\nChild %d terminated\n", pid);
}

void echo(int sockfd)
{
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;

	bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); // blocking
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");

	char *token = malloc(BUFF_SIZE * sizeof(char));
	strcpy(token, buff);
	char docs_path_split_token[20][BUFF_SIZE + 1];
	int count = 0;

	token = strtok(token, "/");

	while (token != NULL)
	{
		strcpy(docs_path_split_token[count], token);
		token = strtok(NULL, "/");
		count++;
	}

	char file[strlen(docs_path_split_token[count - 1]) + 1];
	strcpy(file, docs_path_split_token[count - 1]);
	char *file_name = strtok(file, ".");

	int i = 0;
	while (file[i] != '\0')
	{
		if (file[i] >= 'a' && file[i] <= 'z')
		{
			file[i] = file[i] - 32;
		}
		i++;
	}
	strcat(file, ".txt");
	char *newPath = malloc(BUFF_SIZE * sizeof(char));
	strcat(newPath, "/");

	for (int i = 0; i < count - 1; i++)
	{
		strcat(newPath, docs_path_split_token[i]);
		strcat(newPath, "/");
	}
	strcat(newPath, file);

	FILE *ptr_in, *ptr_out;
	ptr_in = fopen(buff, "r+");
	ptr_out = fopen(newPath, "w");
	char ch;

	if (ptr_in == NULL)
	{
		perror("\nError: \n");
	}
	else
	{
		while (ch != EOF)
		{
			ch = fgetc(ptr_in);
			if ((ch >= 'a') && (ch <= 'z'))
			{
				ch = ch - 32;
				fputc(ch, ptr_out);
			}
			else
				fputc(ch, ptr_out);
		}
	}

	fseek(ptr_out, 0L, SEEK_END);

	// calculating the size of the file
	long int res = ftell(ptr_out);
	printf("%ld", res);

	fclose(ptr_in);
	fclose(ptr_out);

	bytes_sent = send(sockfd, newPath, strlen(newPath), 0); /* echo to the client */
	if (bytes_sent < 0)
		perror("\nError: ");
	char sres[BUFF_SIZE + 1];
	// 	convert res to string [buf]
	itoa(res, sres);
	bytes_sent = send(sockfd, sres, strlen(sres), 0); /* echo to the client */
	if (bytes_sent < 0)
		perror("\nError: ");

	close(sockfd);
}

void reverse(char s[])
{
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void itoa(int n, char s[])
{
	int i, sign;

	if ((sign = n) < 0) /* record sign */
		n = -n;			/* make n positive */
	i = 0;
	do
	{						   /* generate digits in reverse order */
		s[i++] = n % 10 + '0'; /* get next digit */
	} while ((n /= 10) > 0);   /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}