#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio_ext.h>

// Remember to use -pthread when compiling this server's source code
void *connection_handler(void *);

int main()
{
	int server_socket, sin_size;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client;

	if (server_socket == -1)
	{
		perror("Socket initialisation failed");
		exit(EXIT_FAILURE);
	}
	else
		printf("Server socket created successfully\n");

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5500);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to the specified IP addr and port
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
	{
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	if (listen(server_socket, 3) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");

	int no_threads = 0;
	pthread_t threads[3];
	while (no_threads < 3)
	{
		printf("Listening...\n");
		sin_size = sizeof(struct sockaddr_in);

		int client_socket = accept(server_socket, (struct sockaddr *)&client, &sin_size);
		puts("Connection accepted");
		if (pthread_create(&threads[no_threads], NULL, connection_handler, (void *)&client_socket) < 0)
		{
			perror("Could not create thread");
			return 1;
		}
		if (client_socket < 0)
		{
			printf("server acccept failed...\n");
			exit(0);
		}
		else
			printf("Server acccept the client...\n");
		puts("Handler assigned");
		no_threads++;
	}
	int k = 0;
	for (k = 0; k < 3; k++)
	{
		pthread_join(threads[k], NULL);
	}

	// int send_status;
	// send_status=send(client_socket, server_message, sizeof(server_message), 0);
	close(server_socket);

	return 0;
}

void *connection_handler(void *client_socket)
{
	int socket = *(int *)client_socket;
	int read_len;
	int send_status;

	char client_message[1024];

	while (1)
	{
		read_len = recv(socket, client_message, 1024, 0);
		if (read_len <= 0)
		{
			break;
		}
		int is_text = 1, i;
		client_message[read_len] = '\0';
		
		if (strcmp(client_message, "Q") == 0 || strcmp(client_message, "q") == 0)
		{
			send(socket, "exit", strlen("exit"), 0);
			break;
		}
		for (i = 0; client_message[i] != '\0'; i++)
		{
			if (client_message[i] >= 'a' && client_message[i] <= 'z')
			{
				client_message[i] = client_message[i] - 32;
			}
			else
			{
				if (client_message[i] >= 'A' && client_message[i] <= 'Z')
				{
					continue;
				}
				else
				{
					strcpy(client_message, "Wrong text format");
					break;
				}
			}
		}

		// Send the message back to client

		send_status = send(socket, client_message, strlen(client_message), 0);
	}

	return 0;
}
