#include "file_transfer.h"

void list_dir(int client_socket, const char *buffer, const struct sockaddr_in *client_addr, socklen_t addr_len)
{
    struct dirent *ent;
    char dir_path[BUFFER_SIZE];
    char directory_listing[BUFFER_SIZE];
    DIR *dir;

    memset(dir_path, 0, BUFFER_SIZE);
    memset(directory_listing, 0, BUFFER_SIZE);

    strncpy(dir_path, ".", BUFFER_SIZE - 1);
    if (buffer[1] != '\0')
    {
        strncat(dir_path, "/", BUFFER_SIZE - strlen(dir_path) - 1);
        strncat(dir_path, buffer + 1, BUFFER_SIZE - strlen(dir_path) - 1);
    }

    dir = opendir(dir_path);

    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL) 
        {
            if (strlen(directory_listing) + strlen(ent->d_name) + 2 > BUFFER_SIZE)
            {
            
                break;
            }
            strncat(directory_listing, ent->d_name, BUFFER_SIZE - strlen(directory_listing) - 1);
            strncat(directory_listing, "\n", BUFFER_SIZE - strlen(directory_listing) - 1);
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
        strncpy(error_packet + 1, error_msg, BUFFER_SIZE - 1);
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
    char *cwd = NULL;
    size_t cwd_size = 0;

    // Get the size of the current working directory
    ssize_t len = -1;
    while (len < 0) {
        cwd_size += 100; // Incremental allocation size
        cwd = realloc(cwd, cwd_size);
        len = getcwd(cwd, cwd_size);
    }

    // Check for errors
    if (len == -1)
    {
        char error_msg[] = "Error getting current working directory";
        char error_packet[BUFFER_SIZE];
        error_packet[0] = CMD_ERR;

        strcpy(error_packet + 1, error_msg);

        sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
        
    }

    // Send the current working directory
    sendto(client_socket, cwd, strlen(cwd), 0, (struct sockaddr *)client_addr, addr_len);

    // Free dynamically allocated memory
    free(cwd);
}

void upload_request(int client_socket, const char *buffer, const struct sockaddr_in *client_addr, socklen_t addr_len)
{
    char filename[BUFFER_SIZE];
    strcpy(filename, buffer + 1);

    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        char error_msg[] = "Error opening file for writing!";
        char error_packet[BUFFER_SIZE];
        error_packet[0] = 0x5;
        strcpy(error_packet + 1, error_msg);
        sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
        return;  // Exit the function if file opening failed
    }

    int total_packets;
    if (recvfrom(client_socket, (char *)&total_packets, sizeof(total_packets), 0, NULL, NULL) < 0)
    {
        char error_msg[] = "Error receiving total packet count!";
        char error_packet[BUFFER_SIZE];
        error_packet[0] = 0x5;
        strcpy(error_packet + 1, error_msg);
        sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
        fclose(file);
        return;  // Exit the function if receiving total packets failed
    }

    for (int packet_number = 1; packet_number <= total_packets; packet_number++)
    {
        char file_buffer[BUFFER_SIZE];
        ssize_t bytes_received = recvfrom(client_socket, file_buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (bytes_received < 0)
        {
            char error_msg[] = "Error receiving file data!";
            char error_packet[BUFFER_SIZE];
            error_packet[0] = 0x5;
            strcpy(error_packet + 1, error_msg);
            sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)client_addr, addr_len);
            fclose(file);
            return;  // Exit the function if receiving file data failed
        }

        fwrite(file_buffer, 1, bytes_received, file);

        char ack_packet[2] = {0x4, (char)packet_number};
        sendto(client_socket, ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)client_addr, addr_len);
    }

    fclose(file);
}



