#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#define IS_STRING 1
#define IS_NUMBER 2
#define MAX_CHAR 100

int main(int argc, char *argv[])
{

    int type = IS_NUMBER;
    char param[MAX_CHAR];
    strcpy(param, argv[1]);

    if (argc != 2)
    {
        printf("Wrong command\n");
        return;
    }
    else
    {
        // Check Param is address or IP
        type = check_input_address(param);
    }

    // address is having string
    if (type == IS_STRING)
    {
        get_data(argv[1]); // get data due to address
    }// IP is having number only
    else if (type == IS_NUMBER)
    {
        get_data_2(argv[1]); // get data due to IP
    }
}

// check is address or IP function
int check_input_address(char *parameter)
{
    char *token = strtok(parameter, ".");
    while (token != NULL)
    {
        for (int i = 0; i < strlen(token); i++)
        {
            if ((token[i] >= 65 && token[i] <= 90) || (token[i] >= 97 && token[i] <= 122))
            {
                return IS_STRING;
            }
        }
        token = strtok(NULL, ".");
    }
    return IS_NUMBER;
}

// get data by address function
void get_data(char *parameter)
{
    struct hostent *he;
    struct in_addr **addr_list;

    he = gethostbyname(parameter);
    if (he == NULL)
    {
        printf("No Information found\n");
    }
    else
    {
        addr_list = (struct in_addr **)he->h_addr_list;

        for (int i = 0; addr_list[i] != NULL; i++)
        {
            if (i == 0)
            {
                printf("Official IP: %s \n", inet_ntoa(*addr_list[i]));
                printf("Alias IP:\n");
            }
            else
            {
                printf("%s \n", inet_ntoa(*addr_list[i]));
            }
        }
    }
}

// get data by IP address function
void get_data_2(char *parameter)
{
    struct hostent *he;
    struct in_addr addr;
    inet_aton(parameter, &addr);

    he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he == NULL)
    {
        printf("No Information found\n");
    }
    else
    {

        printf("Official IP: %s \n", he->h_name);
        printf("Alias IP:");

        if (he->h_aliases[0] == NULL)
        {
            printf(" NULL\n");
        }
        else
        {
            for (int i = 0; he->h_aliases[i] != NULL; i++)
            {
                printf("%s", he->h_aliases[i]);
            }
        }
    }
}