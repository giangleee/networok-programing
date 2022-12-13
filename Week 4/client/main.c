/*UDP Echo Client*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 1024

#define TRUE 1
#define FALSE 0
#define MAX_CHAR 100

int checkValidString(char *param)
{
    int checkIsValid = TRUE;
    for (int i = 0; i < strlen(param) - 1; i++)
    {
        if (param[i] < 48 ||
            (param[i] > 57 && param[i] < 65) ||
            (param[i] > 90 && param[i] < 97) ||
            param[i] > 122)
        {
            checkIsValid = FALSE;
            break;
        }
    }

    if (checkIsValid)
    {
        return TRUE;
    }
    return FALSE;
}

int main(int argc, char *argv[])
{
    // IP and PORT detail
    int serverPort;
    char serverIp[MAX_CHAR];
    int client_sock;
    char buff[BUFF_SIZE];
    struct sockaddr_in server_addr_client;
    int bytes_sent, bytes_received_string, bytes_received_int, sin_size;

    // Step 0: Get data from command
    if (argc != 3)
    {
        printf("Wrong command!\n");
        exit(0);
    }
    else
    {
        serverPort = atoi(argv[2]);
        strcpy(serverIp, argv[1]);
    }

    // Step 1: Construct a UDP socket
    if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    { /* calls socket() */
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Define the address of the server by first client
    bzero(&server_addr_client, sizeof(server_addr_client));
    server_addr_client.sin_family = AF_INET;
    server_addr_client.sin_port = htons(serverPort);
    server_addr_client.sin_addr.s_addr = inet_addr(serverIp);

    while (TRUE)
    {
        // Step 3: Communicate with server
        printf("Insert string to send:");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        int isValid = checkValidString(buff);

        if (!isValid)
        {
            printf("Error!\n");
            exit(0);
        }

        sin_size = sizeof(struct sockaddr);

        bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr_client, sin_size);
        if (bytes_sent < 0)
        {
            perror("Error: ");
            close(client_sock);
            return 0;
        }

        // print int value
        int buff_int;
        bytes_received_int = recvfrom(client_sock, &buff_int, sizeof(buff), 0, (struct sockaddr *)&server_addr_client, &sin_size);
        if (bytes_received_int < 0)
        {
            perror("Error: ");
            close(client_sock);
            return 0;
        }
        printf("%d\n", buff_int);

        // print string value
        bytes_received_string = recvfrom(client_sock, buff, sizeof(buff), 0, (struct sockaddr *)&server_addr_client, &sin_size);
        if (bytes_received_string < 0)
        {
            perror("Error: ");
            close(client_sock);
            return 0;
        }
        printf("%s", buff);
    }

    close(client_sock);
    return 0;
}
