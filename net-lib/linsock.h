#ifndef __LIN_SOCK__
#define __LIN_SOCK__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int ol_socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in ol_server_bind(int socket, int port, char* addr)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(socket, (struct sockaddr*)&server_addr, sizeof(server_addr) < 0)) {
        perror("Bind failed");
        close(socket);
        exit(1);
    }
}
#endif