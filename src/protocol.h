#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <arpa/inet.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void handle_client_request(int client_socket);

#endif
