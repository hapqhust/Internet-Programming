/*UDP Echo Server*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORT 5500 /* Port that will be opened */
#define BUFF_SIZE 1024

typedef struct Message
{
	char digit[BUFF_SIZE];
	char str[BUFF_SIZE];
	int isValid;
} Message;

Message *CheckMessage(char *buff, int length)
{
	int i = 0;
	int digit_index = 0, str_index = 0;
	Message *temp = (Message *)malloc(sizeof(Message));
	temp->isValid = 1;
	while (i < length)
	{
		if (buff[i] <= '9' && buff[i] >= '0')
		{
			temp->digit[digit_index] = buff[i];
			digit_index++;
		}
		else
		{
			if ((buff[i] <= 'z' && buff[i] >= 'a') || (buff[i] <= 'Z' && buff[i] >= 'A'))
			{
				temp->str[str_index] = buff[i];
				str_index++;
			}
			else
			{
				temp->isValid = 0;
				return temp;
			}
		}
		i++;
	}
	return temp;
}

int main(int argc, char *argv[])
{

	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received_client1, bytes_received_client2, bytes_received_client_x;
	struct sockaddr_in server;					   /* server's address information */
	struct sockaddr_in client1, client2, client_x; /* client's address information */
	int sin_size;
	char line_break[] = "\n";

	if (argc != 2)
	{
		perror("Error Input.\n");
		exit(0);
	}

	// Step 1: Construct a UDP socket
	if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	// Step 2: Bind address to socket
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1])); /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY;	/* INADDR_ANY puts your IP address automatically */
	bzero(&(server.sin_zero), 8);			/* zero the rest of the structure */

	if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}

	// Step 3: Communicate with clients
	while (1)
	{
		Message *p = (Message *)malloc(sizeof(Message));
		sin_size = sizeof(struct sockaddr_in);

		/* Client 1 connect to server*/
		bytes_received_client1 = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client1, &sin_size);
		if (bytes_received_client1 < 0)
			perror("\nError: ");
		else
		{
			printf("Client 1 da ket noi thanh cong vs server %s:%d\n", inet_ntoa(client1.sin_addr), ntohs(client1.sin_port));
			bytes_sent = sendto(server_sock, "done", 4, 0, (struct sockaddr *)&client1, sin_size); /* send to the client welcome message */
			if (bytes_sent < 0)
				perror("\nError: ");
		}

		/* Client 2 connect to server*/
		bytes_received_client2 = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client2, &sin_size);
		if (bytes_received_client2 < 0)
			perror("\nError: ");
		else
		{
			printf("Client 2 da ket noi thanh cong vs server %s:%d\n", inet_ntoa(client2.sin_addr), ntohs(client2.sin_port));
			bytes_sent = sendto(server_sock, "done", 4, 0, (struct sockaddr *)&client2, sin_size); /* send to the client welcome message */
			if (bytes_sent < 0)
				perror("\nError: ");
		}

		//============================================================

		while (1)
		{
			bytes_received_client_x = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client_x, &sin_size);
			if (bytes_received_client_x < 0)
				perror("\nError: ");
			else
			{
				buff[bytes_received_client_x] = '\0';

				// Kiểm tra xem có phải xâu rổng không, nếu có thì dừng server và 2 client
				if (bytes_received_client_x <= 1)
				{
					char mess[BUFF_SIZE];
					char exit[] = "exit";
					strcpy(mess, exit);
					bytes_sent = sendto(server_sock, mess, strlen(mess), 0, (struct sockaddr *)&client2, sin_size); /* send to the client welcome message */
					if (bytes_sent < 0)
						perror("\nError: ");
					bytes_sent = sendto(server_sock, mess, strlen(mess), 0, (struct sockaddr *)&client1, sin_size); /* send to the client welcome message */
					if (bytes_sent < 0)
						perror("\nError: ");
					return 0;
				}

				p = CheckMessage(buff, bytes_received_client_x - 1); // do dai xau nhan duoc la bytes_received-1

				// Xử lý message từ client để gửi cho client còn lại
				char mess[BUFF_SIZE];
				strcpy(mess, line_break);

				if (p->isValid == 0)
				{
					char error[] = "Error";
					strcat(mess, error);
				}
				else
				{
					if (strlen(p->digit) > 0)
					{
						strcat(mess, p->digit);
						strcat(mess, line_break);
					}
					strcat(mess, p->str);
				}
				strcat(mess, line_break);

				//Gửi message tới client còn lại
				if (ntohs(client1.sin_port) == ntohs(client_x.sin_port))
				{
					bytes_sent = sendto(server_sock, mess, strlen(mess), 0, (struct sockaddr *)&client2, sin_size); /* send to the client welcome message */
					if (bytes_sent < 0)
						perror("\nError: ");
				}
				else
				{
					bytes_sent = sendto(server_sock, mess, strlen(mess), 0, (struct sockaddr *)&client1, sin_size); /* send to the client welcome message */
					if (bytes_sent < 0)
						perror("\nError: ");
				}
			}
		}
		free(p);
	}

	close(server_sock);
	return 0;
}
