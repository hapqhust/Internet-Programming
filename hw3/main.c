#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

typedef struct hostent hostent;

struct hostent *he;
struct in_addr **addr_list;
struct in_addr addr;

void convert_IP_into_DNS(char *name)
{
    inet_aton(name, &addr);
    he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he == NULL)
    {                                     // do some error checking
        printf("No Information found\n"); // herror(), NOT perror()
        exit(0);
    }

    printf("Offical name: %s\n", he->h_name);
    printf("Alias name:\n");
    int i;
    for (i = 0; he->h_aliases[i] != NULL; i++)
    {
        printf("%s\n", he->h_aliases[i]);
    }
    printf("\n");
}

void convert_DNS_into_IP(char *name)
{
    he = gethostbyname(name);
    if (he == NULL)
    {                                     // do some error checking
        printf("No Information found\n"); // herror(), NOT perror()
        exit(0);
    }

    // print information about this host:
    // printf("Official name is: %s\n", he->h_name);
    printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)he->h_addr));
    printf("Alias IP:\n");
    addr_list = (struct in_addr **)he->h_addr_list;

    int i;
    for (i = 0; addr_list[i] != NULL; i++)
    {
        printf("%s\n", inet_ntoa(*addr_list[i]));
    }
    printf("\n");
}

int check(char *name)
{
    int x;
    char c;
    for (x = 0; x < strlen(name); x++)
    {
        c = name[x];
        if ((c >= '0' && c <= '9') || c == '.')
        {
            continue;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Error Input.\n");
        return 0;
    }

    char name[100];
    strcpy(name, argv[1]);
    if (check(name))
    {
        convert_DNS_into_IP(name);
    }
    else
    {
        convert_IP_into_DNS(name);
    }

    // get the host name of 66.94.230.32:

    // inet_aton("66.94.230.32", &addr);
    // he = gethostbyaddr(&addr, sizeof(addr), AF_INET);

    // printf("Host name: %s\n", he->h_name);
}