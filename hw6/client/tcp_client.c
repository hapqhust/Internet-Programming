#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 10240
#define BUFF_SIZE2 1024

#define FOLDER "home/image/"

int CheckFileFormat(char * filename){
	char *token = strtok(filename, ".");
	token = strtok(NULL, ".");
	if(strcmp(token, "bmp") == 0 || strcmp(token, "jpg") == 0 || strcmp(token, "png") == 0)
		return 1;
	return 0;
}

void receive_image(int socket, char *file_name)
{ 

	int buffersize = 0, recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;

	char imagearray[BUFF_SIZE];
	char verify = '1';
	FILE *image;

	char file_copy[100];
	strcpy(file_copy, file_name);

	// Kiểm tra định dạng của file
	if(!CheckFileFormat(file_copy)){
		printf("\nError: Wrong File Format!\n");
		return;
	}

	// Check file tồn tại
	char path_of_image[100];
	strcat(path_of_image, FOLDER);
	strcat(path_of_image, file_name);
	printf("%s\n", path_of_image);
	if (access(path_of_image, F_OK) != -1){
		printf("\nFile Already Exists!\n");
		return;
	} 

	stat = send(socket, file_name, BUFF_SIZE2, 0);

	// Tìm kích thước của file
	stat = recv(socket, &size, sizeof(int), 0);

	// Size trả về có kích thước nhỏ hơn 0 => Không tìm thấy file
	if (size < 0)
	{
		printf("\nError: File Not Found!\n");
		return;
	}

	// printf("Packet received.\n");
	// printf("Packet size: %i\n", stat);
	// printf("Image size: %i\n", size);
	// printf(" \n");

	char buffer[] = "Got it";

	// Gửi tín hiệu xác nhận
	do
	{
		stat = send(socket, &buffer, sizeof(int), 0);
	} while (stat < 0);

	image = fopen(path_of_image, "w");

	if (image == NULL)
	{
		printf("Error has occurred. Image file could not be opened\n");
		return;
	}

	// Lặp cho đến khi nhận đủ dữ liệu của file

	int need_exit = 0;
	struct timeval timeout = {10, 0};

	fd_set fds;
	int buffer_fd, buffer_out;

	while (recv_size < size)
	{
		// while(packet_index < 2){

		FD_ZERO(&fds);
		FD_SET(socket, &fds);

		buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

		if (buffer_fd < 0)
			printf("error: bad file descriptor set.\n");

		if (buffer_fd == 0)
			printf("error: buffer read timeout expired.\n");

		if (buffer_fd > 0)
		{
			do
			{
				read_size = recv(socket, imagearray, BUFF_SIZE + 1, 0);
			} while (read_size < 0);

			printf("Packet number received: %i\n", packet_index);
			printf("Packet size: %i\n", read_size);

			// Write the currently read data into our image file
			write_size = fwrite(imagearray, 1, read_size, image);
			printf("Written image size: %i\n", write_size);
			printf("")

			if (read_size != write_size)
			{
				printf("error in read write\n");
			}

			// Increment the total number of bytes read
			recv_size += read_size;
			packet_index++;
			printf("Total received image size: %i\n", recv_size);
			printf(" \n");
		}
	}

	fclose(image);
	printf("\nFile Transfer Is Completed…\n");
	return;
}

int main(int argc, char *argv[])
{
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;

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
	char file_name[BUFF_SIZE2];
	strcpy(file_name, argv[3]);

	receive_image(client_sock, file_name);

	// Step 4: Close socket
	close(client_sock);
	return 0;
}
