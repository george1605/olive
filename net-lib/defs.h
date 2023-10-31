#include <Windows.h>
#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define DEFAULT_BUFLEN 512

enum OlPorts {
    HTTP_PORT = 80,
    HTTPS_PORT = 443
};

typedef struct {
    SOCKET listen;
    SOCKET client;
} OlServer;

OlServer ol_init_server() 
{

}

int ol_wsa_startup(WSADATA* data)
{
    return WSAStartup(MAKEWORD(2,2), data);
}

