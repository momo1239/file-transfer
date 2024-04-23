#include <arpa/inet.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void handle_client_request(int client_socket);

int main(int argc, char *argv[])
{
	// Command line arg for port
	if (argc != 2)
	{
		printf("Usage: ./fileserver [port]\n");
		exit(1);
	}
	int PORT = atoi(argv[1]);
	int server_socket;
	struct sockaddr_in server_addr;

	// Create UDP socket

	if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Socket creation failed");
		exit(1);
	}

	// Init server addr structure

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// Bind socket

	if (bind(server_socket, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Socket binding failed");
		exit(EXIT_FAILURE);
	}

	printf("Server is running...\n");

	// Loop for receiving commands

	while (1)
	{
		handle_client_request(server_socket);

	}
	close(server_socket);

	return 0;

}

void handle_client_request(int client_socket)
{
	char buffer[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	// Receive command from client

	recvfrom(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
	printf("Received command from client: %s\n", buffer);


	// Check command for DIR functionality

	if (strncmp(buffer, "DIR", 3) == 0)
	{
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(".")) != NULL)
		{
			// Create buffer to add list of files
			char directory_listing[BUFFER_SIZE];
			memset(directory_listing, 0, BUFFER_SIZE);

			// Append dir listing to buffer via readdir

			while ((ent = readdir(dir)) != NULL)
			{
				strcat(directory_listing, ent->d_name);
				strcat(directory_listing, "\n");
			}

			// Send dir listing to client

			sendto(client_socket, directory_listing, strlen(directory_listing), 0, (struct sockaddr *)&client_addr, addr_len);

			closedir(dir);
		}
		else
		{
			perror("Could not list dir");
			exit(1);
		}
	}


}
