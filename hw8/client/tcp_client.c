#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024

int receive_file(int socket)
{
	int buffersize = 0, recv_size = 0, size = -1, read_size, write_size, packet_index = 1, stat;

	char filearray[BUFF_SIZE];
	char verify = '1';
	FILE *file;

	// Tìm kích thước của file
	do
	{
		stat = recv(socket, &size, sizeof(int), 0);
	} while (stat < 0);

	// Size trả về có kích thước nhỏ hơn 0 => Không tìm thấy file
	// printf("%d\n", size);
	if (size <= 0)
	{
		printf("\nLOGIN IS UNSUCCESSFUL!\n");
		return 0;
	}

	char buffer[] = "Got it";

	// Gửi tín hiệu xác nhận
	do
	{
		stat = send(socket, &buffer, sizeof(buffer), 0);
	} while (stat < 0);

	file = fopen("chat.txt", "w");

	if (file == NULL)
	{
		printf("Error has occurred. file file could not be opened\n");
		return 0;
	}

	// Lặp cho đến khi nhận đủ dữ liệu của file
	while (recv_size < size)
	{
		do
		{
			read_size = recv(socket, filearray, BUFF_SIZE + 1, 0);
		} while (read_size < 0);

		// printf("Packet number received: %i\n", packet_index);
		// printf("Packet size: %i\n", read_size);

		// Write the currently read data into our file file
		write_size = fwrite(filearray, 1, read_size, file);
		// printf("Written file size: %i\n", write_size);

		if (read_size != write_size)
		{
			printf("error in read write\n");
		}

		// Increment the total number of bytes read
		recv_size += read_size;
		packet_index++;
		// printf("Total received file size: %i\n", recv_size);
		// printf(" \n");
	}

	fclose(file);
	// printf("\nFile Transfer Is Completed…\n");
	return 1;
}

void ReplaceString(char str[], char substr[], char replace[])
{
	// printf("%s-%s-%s", str, substr, replace);
	char output[128];
	int i = 0, j = 0, flag = 0, start = 0;

	str[strlen(str) - 1] = '\0';
	// substr[strlen(substr) - 1] = '\0';
	// replace[strlen(replace) - 1] = '\0';

	// check whether the substring to be replaced is present
	while (str[i] != '\0')
	{
		if (str[i] == substr[j])
		{
			if (!flag)
				start = i;
			j++;
			if (substr[j] == '\0')
				break;
			flag = 1;
		}
		else
		{
			flag = start = j = 0;
		}
		i++;
	}
	if (substr[j] == '\0' && flag)
	{
		for (i = 0; i < start; i++)
			output[i] = str[i];

		// replace substring with another string
		for (j = 0; j < strlen(replace); j++)
		{
			output[i] = replace[j];
			i++;
		}
		// copy remaining portion of the input string "str"
		for (j = start + strlen(substr); j < strlen(str); j++)
		{
			output[i] = str[j];
			i++;
		}
		// print the final string
		output[i] = '\0';
		printf("%s\n", output);
	}
	else
	{
		printf("%s\n", str);
	}
}

void ReadFile(char *username)
{
	FILE *inp = fopen("chat.txt", "r");
	if (!inp)
	{
		printf("Error: Can't open this file! \n");
		return;
	}

	char str[BUFF_SIZE];
	while (fgets(str, BUFF_SIZE, inp) != NULL)
	{
		/* Ghi noi dung len stdout */
		ReplaceString(str, username, "you:");
	}
	fclose(inp);
	return;
}

int main(int argc, char *argv[])
{
	int client_sock;
	char buff[BUFF_SIZE + 1];
	struct sockaddr_in server_addr, client_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;

	if (argc != 5)
	{
		printf("\nInput Error");
		return 0;
	}

	// Step 1: Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	// Step 3: Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

	// Step 4: Communicate with server

	char login[BUFF_SIZE], username[BUFF_SIZE];
	strcpy(login, argv[3]);
	strcpy(username, argv[3]);

	strcat(username, ":");
	strcat(login, " ");
	strcat(login, argv[4]);
	bytes_sent = send(client_sock, login, strlen(login), 0);
	if (bytes_sent < 0)
		perror("\nError: ");

	// receive echo reply

	if (!receive_file(client_sock))
	{
		close(client_sock);
		exit(0);
	}
	else
	{
		ReadFile(username);
	}

	// send message
	while (1)
	{
		printf("\nInsert string to send:");
		memset(buff, '\0', (strlen(buff) + 1));
		fgets(buff, BUFF_SIZE, stdin);
		msg_len = strlen(buff);

		bytes_sent = send(client_sock, buff, msg_len, 0);
		if (bytes_sent < 0)
			perror("\nError: ");
		if (strcmp(buff, "bye") == 0)
		{
			break;
		}
		// receive echo reply
		if (!receive_file(client_sock))
		{
			close(client_sock);
			exit(0);
		}
		else
		{
			ReadFile(username);
		}
	}

	// Step 4: Close socket
	close(client_sock);
	return 0;
}
