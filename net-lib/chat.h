#ifndef __CHAT__
#define __CHAT__
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.c"
#define OL_MAX_CLIENTS 1000

typedef struct {
    void* socket;
    char* name;
} OlClient;

typedef struct {
    OlClient clients[OL_MAX_CLIENTS];
    size_t nr_clients;
    void* serv_socket;
} OlServer;

void ol_remove_client(OlClient* client)
{
    ol_net_close(client->socket);
}

void ol_broadcast(OlServer server, char* msg)
{
    for(int i = 0;i < server.nr_clients;i++)
    {
        if(server.clients[i].socket != NULL)
            ol_net_send(server.clients[i].socket, msg, strlen(msg));
    }
}

#endif