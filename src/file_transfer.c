#include "file_transfer.h"

void list_dir(int client_socket, const char *buffer, const struct sockaddr_in *client_addr, socklen_t addr_len)
{
	struct dirent *ent;
	char dir_path[BUFFER_SIZE];
	char directory_listing[BUFFER_SIZE];
	DIR *dir;

	memset(dir_path, 0, BUFFER_SIZE);
	memset(directory_listing, 0, BUFFER_SIZE);

	strcpy(dir_path, ".");
	if (buffer[1] != '\0')
	{
		strcat(dir_path, "/");
		strcat(dir_path, buffer + 1);
	}

	dir = opendir(dir_path);

	if (dir != NULL)
	{
		while ((ent = readdir(dir)) != NULL) 
		{
			strcat(directory_listing, ent->d_name);
			strcat(directory_listing, "\n");
		}
		size_t dir_listing_size = strlen(directory_listing);

		char size_str[20];
		snprintf(size_str, sizeof(size_str), "%zu", dir_listing_size);

		sendto(client_socket, size_str, strlen(size_str), 0, (struct sockaddr *)client_addr, addr_len);
		sendto(client_socket, directory_listing, strlen(directory_listing), 0, (struct sockaddr *)client_addr, addr_len);

		closedir(dir);
	}
	else
	{
		char error_msg[] = "Error listing directory!";
		char error_packet[BUFFER_SIZE];
		error_packet[0] = CMD_ERR;
		strcpy(error_packet + 1, error_msg);
		sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
	}
}

void get_request(int client_socket, const char *buffer, const struct sockaddr_in *client_addr, socklen_t addr_len)
{
	char filename[BUFFER_SIZE];
	strcpy(filename, buffer + 1);

	FILE *file = fopen(filename, "rb");
	if (file == NULL)
	{
		char error_msg[] = "File not found!";
		char error_packet[BUFFER_SIZE];
		error_packet[0] = 0x5;
		strcpy(error_packet + 1, error_msg);
		sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
	}
	else
	{
		fseek(file, 0, SEEK_END);
		long file_size = ftell(file);
		int packet_count = (file_size + BUFFER_SIZE - 1) / BUFFER_SIZE;
		fseek(file, 0, SEEK_SET);

		char packet_count_str[20];
		snprintf(packet_count_str, sizeof(packet_count_str), "%d", packet_count);
		sendto(client_socket, packet_count_str, strlen(packet_count_str), 0, (struct sockaddr *)client_addr, addr_len);

		char file_buffer[BUFFER_SIZE];
		int packet_number = 1;
		int ack_packet_number;
		size_t bytes_read;

		while ((bytes_read = fread(file_buffer, 1, BUFFER_SIZE, file)) > 0)
		{
			sendto(client_socket, file_buffer, bytes_read, 0, (struct sockaddr *)client_addr, addr_len);
			printf("Send packet %i\n", packet_number);

			while (1)
			{
				recvfrom(client_socket, (char *)buffer, BUFFER_SIZE, 0, NULL, NULL);
				if (buffer[0] == CMD_ACK)
				{
					ack_packet_number = buffer[1];
					break;
				}
			}
			if (ack_packet_number != packet_number)
			{
				fseek(file, -bytes_read, SEEK_CUR);
				continue;
			}
			printf("Received ACK for packet %i\n", packet_number);

			packet_number++;

			if (packet_number > packet_count)
			{
				break;
			}

		}
		fclose(file);
	}
}


void change_directory(int client_socket, const char *buffer, const struct sockaddr_in *client_addr, socklen_t addr_len) 
{
    char new_dir[BUFFER_SIZE];
    memset(new_dir, 0, sizeof(new_dir));
    strcpy(new_dir, buffer + 1); 

 
    if (chdir(new_dir) == 0)
    {
      
        char success_msg[] = "Directory changed successfully!";
        char success_packet[BUFFER_SIZE];
        success_packet[0] = CMD_ACK;

        strcpy(success_packet + 1, success_msg);

        sendto(client_socket, success_packet, strlen(success_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
    }
    else
    {
       
        char error_msg[] = "Error changing directory!";
        char error_packet[BUFFER_SIZE];
        error_packet[0] = CMD_ERR;

        strcpy(error_packet + 1, error_msg);

        sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
    }
}

void pwd(int client_socket, const struct sockaddr_in *client_addr, socklen_t addr_len)
{
    char cwd[BUFFER_SIZE];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        char error_msg[] = "Error getting current working directory";
        char error_packet[BUFFER_SIZE];
        error_packet[0] = CMD_ERR;

        strcpy(error_packet + 1, error_msg);

        sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
        
    }


    sendto(client_socket, cwd, strlen(cwd), 0, (struct sockaddr *)client_addr, addr_len);
}
