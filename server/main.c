#include "../protocol/protocol.h"

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
		exit(1);
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

