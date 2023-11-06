#ifndef __WIN_SOCK__
#define __WIN_SOCK__
#include "../include/errors.h"
#include <Windows.h>
#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define DEFAULT_BUFLEN 512

typedef struct {
    SOCKET listen; // the listen socket
    SOCKET client; // the client socket
} OlServer;

void ol_server_accept(OlServer server)
{
    server.client = accept(server.listen, NULL, NULL);
}

/*
* Closes both the server and client sockets
*/
void ol_server_cleanup(OlServer server)
{
    closesocket(server.listen);
    closesocket(server.client);         
}

WSADATA ol_wsa_startup()
{
    WSADATA data;
    if(WSAStartup(MAKEWORD(2,2), &data) < 0)
    {
        printf("WSAStartup() error: %d", WSAGetLastError());
        ol_error = OL_ENETWSA;
        return (WSADATA){NULL};
    }
    return data;
}

#endif