#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "account.h"

#define PORT 5550 /* Port that will be opened */
#define BACKLOG 2 /* Number of allowed connections */
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{

	int listen_sock, conn_sock; /* file descriptors */
	char recv_username[BUFF_SIZE], recv_pass[BUFF_SIZE], recv_bye[BUFF_SIZE];
	int bytes_sent, bytes_received_username, bytes_received_pass, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;

	// Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	// Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));		/* Remember htons() from "Conversions" section? =) */
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

	//**Read Account info
	Image *acc = NULL;
	acc = ReadAccount(acc);

	while (1)
	{
		// accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
			perror("\nError: ");

		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

		// start conversation
		while (1)
		{
			// receives message from client
			bytes_received_username = recv(conn_sock, recv_username, BUFF_SIZE - 1, 0); // blocking
			bytes_received_pass = recv(conn_sock, recv_pass, BUFF_SIZE - 1, 0);			// blocking

			if (bytes_received_username <= 0 || bytes_received_pass <= 0)
			{
				printf("\nConnection closed");
				break;
			}
			else
			{
				recv_username[bytes_received_username - 1] = '\0';
				recv_pass[bytes_received_pass - 1] = '\0';
				printf("Receive: %s \n", recv_username);
				printf("Receive: %s \n", recv_pass);
			}

			int n = 1;
			if (!CheckUser(acc, recv_username)) // User ko tồn tại
			{
				printf("Cannot find account \n");
				break;
			}
			else
			{
				if (!CheckActive(acc, recv_username)) // Blocked or inactive
				{
					bytes_sent = send(conn_sock, "Account is blocked or inactive", 31, 0);
				}
				else
				{
					while (n <= 3)
					{
						if (!CheckPassword(acc, recv_pass))
						{
							bytes_sent = send(conn_sock, "Password is not correct. Please try again!", 43, 0);
							bytes_received_pass = recv(conn_sock, recv_pass, BUFF_SIZE - 1, 0); // blocking
							if (bytes_received_pass <= 0)
							{
								printf("Connection closed\n");
								break;
							}
							else
							{
								recv_pass[bytes_received_pass - 1] = '\0';
								printf("Receive: %s \n", recv_pass);
							}
							n++;
						}
						else
						{
							bytes_sent = send(conn_sock, "Login is successful!", 21, 0);
							break;
						}
					}
					if (n == 4)
					{
						bytes_sent = send(conn_sock, "Account is blocked!", 20, 0);
						if (bytes_sent <= 0)
						{
							printf("Connection closed\n");
							break;
						}
						Image *cur = acc;
						while (cur != NULL)
						{
							if (strcmp(cur->username, recv_username) == 0)
							{
								cur->status = 0;
							}
							cur = cur->next;
						}
						UpdateFile(acc);
					}

					do
					{
						bytes_received = recv(conn_sock, recv_bye, BUFF_SIZE - 1, 0);
						if (bytes_received <= 0)
						{
							printf("Error\n");
							break;
						}
						else
						{
							recv_bye[bytes_received - 1] = '\0';
							printf("Receive: %s \n", recv_bye);
							char bye[] = "Bye";
							if (strcmp(recv_bye, bye) == 0)
							{
								char goodbye[] = "Goodbye ";
								strcat(goodbye, recv_username);
								printf("%s \n", goodbye);
								bytes_sent = send(conn_sock, goodbye, strlen(goodbye), 0);
								break;
							}
							else
							{
								bytes_sent = send(conn_sock, recv_bye, bytes_received, 0);
								if (bytes_sent <= 0)
								{
									printf("Connection closed\n");
									break;
								}
							}
						}
					} while (1);
				}
			} // end conversation
			close(conn_sock);
		}

		freeList(acc);
		close(listen_sock);
		return 0;
	}
}
