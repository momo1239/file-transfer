#include "file_transfer.h"
#include "protocol.h"

void handle_client_request(int client_socket)
{
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        // Receive command from client

        recvfrom(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        printf("Received command from client: %i\n", buffer[0]);


        // Check command for DIR functionality

        if (buffer[0] == CMD_DIR)
        {
                list_dir(client_socket, buffer, &client_addr, addr_len);
        }
        else if (buffer[0] == CMD_GET)
        {
                get_request(client_socket, buffer, &client_addr, addr_len);
        }
        else if (buffer[0] == CMD_CD)
        {
                change_directory(client_socket, buffer, &client_addr, addr_len);
        }
        else if (buffer[0] == 0x7)
        {
                pwd(client_socket, &client_addr, addr_len);
        }



}
