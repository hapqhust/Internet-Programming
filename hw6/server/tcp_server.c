#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include "data.h"

#define PORT 5550 /* Port that will be opened */
#define BACKLOG 2 /* Number of allowed connections */
#define BUFF_SIZE 10240

void str_trim_lf(char *arr, int length)
{
	int i;
	for (i = 0; i < length; i++)
	{ // trim \n
		if (arr[i] == '\n')
		{
			arr[i] = '\0';
			break;
		}
	}
}

void send_image(Image *acc, int socket)
{
	FILE *image;
	int size, read_size, stat, packet_index = 1;
	char send_buffer[BUFF_SIZE], read_buffer[256], file_name[1024];

	stat = recv(socket, file_name, 1024, 0);
	str_trim_lf(file_name, 1024);
	printf("%s\n", file_name);

	char path[30];
	strcpy(path, GetPath(acc, file_name));
	if(strcmp(path, "") == 0){
		size = -1;
		send(socket, (void *)&size, sizeof(int), 0);
		return;
	}

	image = fopen(path, "r");

	if (image == NULL)
	{
		printf("Error: Can't opening this image file");
		return;
	}

	fseek(image, 0, SEEK_END);
	size = ftell(image);
	fseek(image, 0, SEEK_SET);
	printf("Total Picture size: %i\n", size);

	// Gửi kích thước ảnh
	printf("Sending Picture Size\n");
	send(socket, (void *)&size, sizeof(int), 0);

	do
	{ 
		stat = recv(socket, &read_buffer, 255, 0);
		printf("Bytes read: %i\n", stat);
	} while (stat < 0);

	while (!feof(image))
	{
		// Read from the file into our send buffer
		read_size = fread(send_buffer, 1, sizeof(send_buffer) - 1, image);

		// Send data through our socket
		do
		{
			stat = send(socket, send_buffer, read_size, 0);
		} while (stat < 0);

		printf("Packet Number: %i\n", packet_index);
		printf("Packet Size Sent: %i\n", read_size);
		printf(" \n");

		packet_index++;

		// Zero out our send buffer
		bzero(send_buffer, sizeof(send_buffer));
	}
	fclose(image);
}

int main(int argc, char *argv[])
{
	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
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
	while (1)
	{
		// accept request
		printf("Waiting for incoming connections...\n");
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
			perror("\nError: ");

		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

		__fpurge(stdout);

		// start conversation

		Image *acc = NULL;
		acc = ReadFile(acc);

		while (1)
		{
			// receives message from client
			send_image(acc, conn_sock);
		} // end conversation
		close(conn_sock);
		freeList(acc);
	}
	close(listen_sock);
	exit(0);
}
