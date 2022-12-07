/*UDP Echo Client*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SERV_PORT 5500
#define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	int client_sock;
	char buff[BUFF_SIZE], connected[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent, bytes_received, sin_size;

	if (argc != 3)
	{
		perror("Error Input.\n");
		exit(0);
	}

	// Step 1: Construct a UDP socket
	if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	// Step 2: Define the address of the server
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	sin_size = sizeof(struct sockaddr);


	// Step 3: Connect to server
	char message[] = "connect";
	bytes_sent = sendto(client_sock, message, strlen(message), 0, (struct sockaddr *)&server_addr, sin_size);
	if (bytes_sent < 0)
	{
		perror("Error: ");
		close(client_sock);
		return 0;
	}

	bytes_received = recvfrom(client_sock, connected, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
	if (bytes_received < 0)
	{
		perror("Error: ");
		close(client_sock);
		return 0;
	}
	connected[bytes_received] = '\0';

	// Step 4: Communicate with server
	while(strcmp(connected, "done") == 0){
		printf("\nInsert string to send:");
		memset(buff, '\0', (strlen(buff) + 1));
		fgets(buff, BUFF_SIZE, stdin);

		bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
		if (bytes_sent < 0)
		{
			perror("Error: ");
			close(client_sock);
			return 0;
		}

		bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
		if (bytes_received < 0)
		{
			perror("Error: ");
			close(client_sock);
			return 0;
		}
		buff[bytes_received] = '\0';
		if (strcmp(buff, "exit") == 0){
			printf("Exit succesfull");
			return 0;
		}
		printf("Reply from server: %s", buff);
	}


	close(client_sock);
	return 0;
}
