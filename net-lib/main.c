#include <stdio.h>
#include "../include/io.c"
#ifdef _WIN32
#include "winsock.h"
#elif defined(__linux__)
#include "linsock.h"
#endif

typedef struct
{
    char* name;
    void* handle;
} OlWifiDevice;

void ol_net_setup()
{
    #ifdef _WIN32
    WSADATA data;
    ol_wsa_startup();
    #else

    #endif
}

void ol_wifi_enumdev(OlWifiDevice* devices, size_t number);
void ol_wifi_connect(OlWifiDevice dev);

void ol_net_send(void*, uint8_t*, size_t);
void ol_net_recv(void*, uint8_t*, size_t);
void ol_net_close(void*);

void __ol_net_send(OlDevice* dev, uint8_t* data, size_t size)
{
    ol_net_send(dev->handle, data, size);
}

void __ol_net_recv(OlDevice* dev, uint8_t* data, size_t size)
{
    ol_net_send(dev->handle, data, size);
}

void ol_net_register(void* socket)
{
    OlDevice dev;
    dev.name = "sock*";
    dev.handle = socket;
    dev.read = __ol_net_recv;
    dev.write = __ol_net_send;
    ol_register_dev(dev);
}

void ol_net_unregister()
{
    OlDevice dev = ol_find_dev("sock*");
    ol_net_close(dev.handle);
}