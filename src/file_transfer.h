#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include <dirent.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

enum Command 
{
	CMD_DIR = 0x1,
	CMD_GET = 0x2,
	CMD_PUT = 0x3,
	CMD_ACK = 0x4,
	CMD_ERR = 0x5,
	CMD_CD = 0x6
};

void list_dir(int client_socket, const char *buffer, const struct sockaddr_in *client_addr, socklen_t addr_len);

void get_request(int client_socket, const char *buffer, const struct sockaddr_in *client_addr, socklen_t addr_len);

void change_directory(int client_socket, const char *buffer, const struct sockaddr_in *client_addr, socklen_t addr_len);

void pwd(int client_socket, const struct sockaddr_in *client_addr, socklen_t addr_len);
#endif
