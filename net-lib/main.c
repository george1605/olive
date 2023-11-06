#include <stdio.h>
#ifdef _WIN32
#include "winsock.h"
#elif defined(__linux__)
#include "linsock.h"
#endif

void ol_net_setup()
{
    #ifdef _WIN32
    WSADATA data;
    ol_wsa_startup();
    #else

    #endif
}

void ol_net_send(void*, char*, size_t);
void ol_net_recv(void*, char*, size_t);
void ol_net_close(void*);