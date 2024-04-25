#include "protocol.h"

void handle_client_request(int client_socket)
{
	char buffer[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int ack_packet_number;

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
	else if (buffer[0] == 0x2)
	{
		// Extract filename
		char filename[BUFFER_SIZE];
		strcpy(filename, buffer + 1); 

		FILE *file = fopen(filename, "rb");
		if (file == NULL)
		{
			// file not found send error packet
			char error_msg[] = "File not found!";
			char error_packet[BUFFER_SIZE];
			error_packet[0] = 0x5;
			strcpy(error_packet + 1, error_msg);
			sendto(client_socket, error_packet, strlen(error_msg) + 1, 0, (struct sockaddr *)&client_addr, addr_len);
		}
		else
		{
			// calculate num of packets needed to send
			fseek(file, 0, SEEK_END);
			long file_size = ftell(file);
			int packet_count = (file_size + BUFFER_SIZE - 1) / BUFFER_SIZE;
			fseek(file, 0, SEEK_SET); // reset file pointer

			// send packet count to client
			char packet_count_str[20];
			snprintf(packet_count_str, sizeof(packet_count_str), "%d", packet_count);
			sendto(client_socket, packet_count_str, strlen(packet_count_str), 0, (struct sockaddr *)&client_addr, addr_len);

			// send file in packet chunks
			char file_buffer[BUFFER_SIZE];
			int packet_number = 1;
			size_t bytes_read;

			while ((bytes_read = fread(file_buffer, 1, BUFFER_SIZE, file)) > 0)
			{
				sendto(client_socket, file_buffer, bytes_read, 0, (struct sockaddr *)&client_addr, addr_len);
				printf("Send packet %i\n", packet_number);

				// wait for ack
				while (1)
				{
					recvfrom(client_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
					if (buffer[0] == 0x4)
					{
						ack_packet_number = buffer[1];
						break;
					}
				}

				if (ack_packet_number != packet_number)
				{
					// if ack is not for packet, resend
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



}
