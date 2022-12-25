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
#include <arpa/inet.h>
#include <stdio_ext.h>

#include "account.h"

#define PORT 5500
#define BACKLOG 20
#define BUFF_SIZE 1024

Account *acc = NULL;

typedef struct ClientNode
{
	int sock;
	struct ClientNode *next;
	char ip[50];
	char name[50];
} ClientList;

ClientList *newNode(int sockfd, char *ip)
{
	ClientList *np = (ClientList *)malloc(sizeof(ClientList));
	np->sock = sockfd;
	np->next = NULL;
	strncpy(np->ip, ip, 16);
	strncpy(np->name, "NULL", 5);
	return np;
}

ClientList *root, *now;

/* Handler process signal*/
void sig_chld(int signo);

/*
 * Receive and echo message to client
 * [IN] sockfd: socket descriptor that connects to client
 */
void echo(int sockfd);

void SendFile(char *filename, int socket);

void WriteToFile(char *username, char *mess);

// void send_message_to_all_clients(ClientList *clt, char mess[]);

int main()
{

	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server;	/* server's address information */
	struct sockaddr_in client;	/* client's address information */
	pid_t pid;
	int sin_size;

	acc = ReadAccount(acc);

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		printf("socket() error\n");
		return 0;
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
	// root = newNode(listen_sock, inet_ntoa(server.sin_addr));
	// now = root;

	/* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);

	while (1)
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
	char buff[BUFF_SIZE], username[BUFF_SIZE], password[BUFF_SIZE];
	int bytes_sent, bytes_received;

	bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); // blocking
	// buff[bytes_received - 1] = '\0';

	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");

	char *s = strtok(buff, " ");
	strcpy(username, s);
	s = strtok(NULL, " ");
	strcpy(password, s);
	// printf("%s-%s\n", username, password);

	if (CheckUser(acc, username) && CheckPassword(acc, password))
	{
		printf("%s join this chatroom\n", username); /* prints client's IP */
		SendFile("groupchat.txt", sockfd);

		while (1)
		{
			bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); // blocking
			buff[bytes_received - 1] = '\0';
			if (bytes_received <= 0 || strcmp(buff, "bye") == 0)
			{
				close(sockfd);
				return;
			}
			else
			{
				WriteToFile(username, buff);
				SendFile("groupchat.txt", sockfd);
			}
		}
	}
	else
	{
		int size = -1;
		bytes_sent = send(sockfd, (void *)&size, sizeof(int), 0); /* echo to the client */
		if (bytes_sent < 0)
			perror("\nError: ");
	}

	close(sockfd);
}

void WriteToFile(char *username, char *mess)
{
	FILE *fptr = fopen("groupchat.txt", "a");
	if (fptr == NULL)
	{
		printf("Error!");
		exit(1);
	}

	fprintf(fptr, "%s: %s\n", username, mess);

	fclose(fptr);
	return;
}

void SendFile(char *filename, int socket)
{
	int size, read_size, stat, packet_index = 1;
	char send_buffer[BUFF_SIZE], read_buffer[256], file_name[1024];

	FILE *file = fopen(filename, "r");

	if (file == NULL)
	{
		printf("Error: Can't opening this file file");
		return;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	printf("Total File size: %i\n", size);

	// Gửi kích thước ảnh
	printf("Sending File Size\n");
	send(socket, (void *)&size, sizeof(int), 0);

	do
	{
		stat = recv(socket, &read_buffer, 255, 0);
		printf("Bytes read: %i\n", stat);
	} while (stat < 0);

	while (!feof(file))
	{
		// Read from the file into our send buffer
		read_size = fread(send_buffer, 1, sizeof(send_buffer) - 1, file);

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
	fclose(file);
}
