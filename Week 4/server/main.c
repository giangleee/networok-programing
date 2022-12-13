/*UDP Echo Server*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 1024

#define TRUE 1
#define FALSE 0

#define MAX_CHAR 100

int getNumber(char *param)
{
    char *numberStringToken = malloc(sizeof(strlen(param) + 1));
    int count = -1;

    for (int i = 0; i < strlen(param); i++)
    {
        if (param[i] >= 48 && param[i] <= 57)
        {
            count++;
            numberStringToken[count] = param[i];
        }
    }

    return atoi(numberStringToken);
}

char *getStringTokenTemp(char *param)
{
    char *stringToken = malloc(sizeof(strlen(param) + 1));
    int count = -1;

    for (int i = 0; i < strlen(param); i++)
    {
        if (param[i] < 48 || param[i] > 57)
        {
            count++;
            stringToken[count] = param[i];
        }
    }
    return stringToken;
}

int main(int argc, char *argv[])
{
    // PORT detail
    int serverPort;  /* Port that will be opened */
    int server_sock; /* file descriptors */
    char buff[BUFF_SIZE];
    int bytes_sent_string, bytes_sent_int, bytes_received;
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in first_client; /* first first_client's address information */
    struct sockaddr_in second_client; /* first second_client's address information */
    int sin_size;
    int firstPort = 1;
    int secondPort = 0;

    // Step 0: Get data from command
    if (argc != 2)
    {
        printf("Wrong command!\n");
        exit(0);
    }
    else
    {
        serverPort = atoi(argv[1]);
    }

    // Step 1: Construct a UDP socket
    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    { /* calls socket() */
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Bind address to socket
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort); /* Remember htons() from "Conversions" section? =) */
    server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY puts your IP address automatically */
    bzero(&(server.sin_zero), 8);        /* zero the rest of the structure */

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    { /* calls bind() */
        perror("\nError: ");
        exit(0);
    }

    // Step 3: Communicate with first first_clients
    while (TRUE)
    {
        sin_size = sizeof(struct sockaddr_in);

        bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&first_client, &sin_size);

        if (bytes_received < 0)
            perror("\nError: ");
        else
        {
            buff[bytes_received] = '\0';
            printf("[%s:%d]: %s", inet_ntoa(first_client.sin_addr), ntohs(first_client.sin_port), buff);
            if (firstPort != secondPort) {
                secondPort = firstPort;
                firstPort = ntohs(first_client.sin_port);
            }
        }
        int buff_int = getNumber(buff);
        char *buff_string = getStringTokenTemp(buff);

        first_client.sin_port = htons(secondPort);
        bytes_sent_int = sendto(server_sock, &buff_int, sizeof(buff_int), 0, (struct sockaddr *)&first_client, sin_size);       /* send to the first_client welcome message */
        bytes_sent_string = sendto(server_sock, buff_string, sizeof(buff_string), 0, (struct sockaddr *)&first_client, sin_size); /* send to the first_client welcome message */

        if (bytes_sent_int < 0 || bytes_sent_string < 0)
        {
            perror("\nError: ");
        }
    }

    close(server_sock);
    return 0;
}
