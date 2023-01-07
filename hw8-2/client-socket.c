#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio_ext.h>
int main(int argc, const char *argv[])
{
    // create a socket
    int network_socket, stat;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5500);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // check for connection_status
    if (connection_status == -1)
    {
        printf("The connection has error\n\n");
    }

    if (connection_status == 0)
    {
        char response[1024], request[1024];
        // receive data from the server
        while (1)
        {
            printf("enter a message to echo\n");
            scanf("%s", request);
            int send_status = send(network_socket, request, strlen(request), 0);
            memset(response, '\0', 1024);
            stat = recv(network_socket, response, 1024, 0);
            response[stat] = '\0';
            if (strcmp(response, "exit") == 0)
            {
                break;
            }
            // print out the server's response
            printf("Here is the echo message from the server: %s\n\n", response);
        }
    }
    // close the socket
    close(network_socket);

    return 0;
}
