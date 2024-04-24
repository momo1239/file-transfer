#include "protocol.h"

void handle_client_request(int client_socket)
{
	char buffer[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	// Receive command from client

	recvfrom(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
	printf("Received command from client: %i\n", buffer[0]);


	// Check command for DIR functionality

	if (buffer[0] == 0x1)
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

			// Calculate size of dir listing

			size_t dir_listing_size = strlen(directory_listing);

			char size_str[20];
			snprintf(size_str, sizeof(size_str), "%zu", dir_listing_size);

			// send size
			sendto(client_socket, size_str, strlen(size_str), 0, (struct sockaddr *)&client_addr, addr_len);

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
