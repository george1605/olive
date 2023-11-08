/*
*   IO.C Source File
*   Contains functions regarding file information, buffers, devices and TTY/COM(USB) communication
*   Under Open Source License
*/
#ifndef __OLIVE_IO__
#define __OLIVE_IO__
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "thread.h"
#define OL_DEVFIND 0x10 // used by ol_open_dev() - finds device
#define OL_DEVNEW  0x20 // used by ol_open_dev() - creates new device
#define OL_NULLDEV (OlDevice){.name = NULL, .handle = NULL}
#define OL_MAX_DEVS 32

typedef struct {
    uint64_t size;
    uint32_t attrs;
} OlFileInfo;

typedef struct _OlDevice {
    char* name;
    void* handle;
    void(*read)(struct _OlDevice*, uint8_t*, size_t);
    void(*write)(struct _OlDevice*, uint8_t*, size_t);
} OlDevice;

typedef struct {
    OlTask* tasks;
    size_t number, size;
    int(*check)(OlIOContext*);
} OlIOContext;

OlIOContext ol_new_context(size_t size)
{
    OlIOContext ctx;
    ctx.tasks = malloc(sizeof(OlTask) * size);
    ctx.number = 0;
    ctx.size = size;
}

void ol_post_context(OlIOContext context, OlTask task)
{
    if(context.number > context.size) return;
    context.tasks[context.number++] = task;
}

void __ol_runctx(OlMsg msg)
{
    OlTask* task = (OlTask*)msg.target;
    if(task == NULL) return;
    ol_run_task(*task);
}

void ol_run_context(OlIOContext context)
{
    // you may not know, but it's an event loop
    int id;
    OlMsgQueue* queue = ol_new_queue(10);
    while(!ol_queue_empty(queue))
    {
        ol_process_event(queue);
        if((id = context.check(&context)) > -1)
            ol_enqueue(queue, __ol_runctx, NULL, &(context.tasks[id])); // adds the new event to the queue
    }
    ol_free_queue(queue);
}

typedef struct
{
    uint8_t* ptr;
    size_t size;
    size_t crs; // cursor
    OlLock lock;
} OlBuffer;

typedef struct {
    char* name;
    void* handle;
    uint8_t* buffer;
    void(*read)(uint8_t*, size_t);
    void(*write)(uint8_t*, size_t);
} OlBufDevice; // buffered device

void ol_get_fileinfo(char* path, OlFileInfo* pInfo)
{
    #ifdef __linux__
        FILE* fp = fopen(path, "w+");
        struct stat finfo;
        fstat(fileno(fp), &finfo);
        pInfo->size = finfo.st_size;
        fclose(fp);
    #elif defined(_WIN32)
        pInfo->attrs = GetFileAttributesA(path);
        WIN32_FILE_ATTRIBUTE_DATA fInfo;
        GetFileAttributesEx(path, GetFileExInfoStandard, &fInfo);
        pInfo->size = fInfo.nFileSizeHigh;
    #endif
}

void ol_lock_bufcopy(OlBuffer* buf, char* data)
{
    ol_wait_lock(&(buf->lock));
    int size = min(buf->size, strlen(data)); // gotcha, even if strlen returns 0xFFFFFFFF, it would not overflow
    memcpy(buf->ptr, data, size);
    buf->crs += size;
    ol_release_lock(buf->lock);
}

// ol_lock_bufread(buf, stdin); - read from stdin
void ol_lock_bufread(OlBuffer* buf, FILE* fp)
{
    if(buf->ptr == NULL) return;
    ol_wait_lock(&(buf->lock));
    size_t bytesRead = fread(buf->ptr, 1, buf->size, fp);
    buf->crs += bytesRead;
    ol_release_lock(buf->lock);
}

void ol_bufget(OlBuffer* buf, OlDevice dev)
{
    if(dev.read != NULL)
        dev.read(&dev, buf->ptr, buf->size);
}

void ol_bufset(OlBuffer buf, FILE* stream)
{
    setvbuf(stream, buf.ptr, _IOFBF, buf.size);
}

// flushes the memory by setting the buffer to NULL and writing directly
void ol_bufflush(OlBuffer* buf, FILE* fp)
{
    setvbuf(fp, NULL, _IONBF, 0);
    fwrite(buf->ptr, 1, buf->size, fp);
    buf->crs = 0; // does not memset() 
}

// exactly like ol_bufflush, but zeroes the memory
void ol_bufflush_z(OlBuffer* buf, FILE* fp)
{
    ol_bufflush(buf, fp);
    memset(buf->ptr, 0, buf->size);
}

void ol_bufzero(OlBuffer* buf)
{
    memset(buf->ptr, 0, buf->size);
}

// copies from buffer -> file buffer -> file
void ol_bufflush_c(OlBuffer* buf, FILE* fp)
{
    fwrite(buf->ptr, 1, buf->size, fp);
    fflush(fp);
    buf->crs = 0;
}

void ol_bufresize(OlBuffer* buf, size_t size)
{
    buf->ptr = realloc(buf->ptr, size);
    buf->size = size;
}

OlBuffer ol_bufcreate(size_t size)
{
    OlBuffer buf;
    buf.size = size;
    buf.ptr = malloc(size);
    buf.crs = 0;
    buf.lock.handle = NULL; // to check later - in ol_bufdelete()
    return buf;
}

OlBuffer ol_lock_bufcreate(size_t size)
{
    OlBuffer buf;
    buf.size = size;
    buf.ptr = malloc(size);
    buf.crs = 0;
    buf.lock = ol_new_lock(); 
    return buf;
}

void ol_bufdelete(OlBuffer buf)
{
    if(buf.lock.handle != NULL) 
        ol_delete_lock(buf.lock);
    free(buf.ptr);
}

typedef struct _OlModule {
    void* handle; // for ol_close_module()
    void(*init)(struct _OlModule*);
    void(*exit)(struct _OlModule*);
} OlModule;

OlModule ol_read_module(char* fname)
{
    OlModule mod;
    #ifdef _WIN32
        HANDLE hnd = LoadLibraryA(fname);
        mod.init = GetProcAddress(hnd, "ol_init");
        mod.exit = GetProcAddress(hnd, "ol_exit");
        mod.handle = hnd;
    #elif defined(__linux__)
        mod.handle = dlopen(fname, RTLD_NOW);
        mod.init = dlsym(mod.handle, "ol_init");
        mod.exit = dlsym(mod.handle, "ol_exit");
    #endif
    return mod;
}

void ol_close_module(OlModule mod)
{
    if(mod.handle == NULL) return;
    #ifdef _WIN32
        CloseHandle(mod.handle);
    #elif defined(__linux__)
        dlclose(mod.handle);
    #endif
}

static OlDevice* ol_regdevs;
static size_t ol_reg_size, ol_reg_cap;

void ol_setup_regdevs(int x)
{
    if(x == 0) x = OL_MAX_DEVS;
    ol_regdevs = malloc(sizeof(ol_regdevs) * x);
    ol_reg_size = 0, ol_reg_cap = x;
}

void ol_register_dev(OlDevice dev)
{
    if(ol_reg_size > ol_reg_cap)
    ol_regdevs[ol_reg_size++] = dev; 
}

OlDevice ol_find_dev(char* name)
{
    for(int i = 0;i < ol_reg_cap;i++)
        if(!strcmp(name, ol_regdevs[i].name))
            return ol_regdevs[i];
    return OL_NULLDEV;
}

OlDevice ol_create_dev(char* name)
{
    OlDevice dev;
    dev.name = strdup(name);
    dev.handle = NULL;
    if(ol_reg_size < ol_reg_cap)
        ol_regdevs[ol_reg_size++] = dev;
    return dev;
}

OlDevice ol_open_dev(char* name, int opts)
{
    switch(opts)
    {
    case OL_DEVFIND:
    return ol_find_dev(name);
    case OL_DEVNEW:
    return ol_create_dev(name);
    }
}

#ifdef _WIN32
#include <windows.h>

typedef struct {
    uint32_t baud;
    uint8_t byte_size;
} OlCOMInfo;

// supports numbers < 10
HANDLE ol_open_com(int number)
{
    char buf[30];
    strcpy(buf, "\\\\.\\COM");
    buf[8] = (number + '0');
    return CreateFileA(buf,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
}

int ol_setup_com(HANDLE hnd, OlCOMInfo info)
{
    DCB dcbSerialParams = { 0 };
    if(info.baud == 0) info.baud = 9600; // default baud
    dcbSerialParams.BaudRate = info.baud; 
    dcbSerialParams.ByteSize = info.byte_size;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    int x = SetCommState(hnd, &dcbSerialParams);
    return x; 
}

void ol_read_com(HANDLE hnd, OlBuffer buf)
{
    int bytesRead;
    ReadFile(hnd, buf.ptr, buf.size, &bytesRead, NULL);
    buf.ptr[bytesRead] = '\0';
}

void ol_write_com(HANDLE hnd, OlBuffer buf)
{
    int bytesWritten;
    WriteFile(hnd, buf.ptr, buf.size, &bytesWritten, NULL);
    // assert(bytesWritten == buf.size)
}

static void __ol_usbread(OlDevice* dev, uint8_t* ptr, size_t size)
{
    OlBuffer buf;
    buf.ptr = ptr;
    buf.size = size;
    ol_read_com(dev->handle, buf);
}

static void __ol_usbwrite(OlDevice* dev, uint8_t* ptr, size_t size)
{
    OlBuffer buf;
    buf.ptr = ptr;
    buf.size = size;
    ol_write_com(dev->handle, buf);
}


// to do this
void ol_register_usb(int number, OlCOMInfo info)
{
    OlDevice dev;
    dev.name = "usb";
    dev.handle = ol_open_com(number);
    dev.read = __ol_usbread;
    dev.write = __ol_usbwrite;
    ol_register_dev(dev);
}

void ol_unregister_usb()
{
    OlDevice dev = ol_find_dev("usb");
    CloseHandle(dev.handle);
}

#endif

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

typedef struct {
    int fd;
    struct termios ttyconfig;
} OlTTYDevice;

typedef struct {
    int parity : 1;
    int attr; // like read/write
    int other : 7;
} OlTTYFlags;

OlTTYDevice ol_open_tty(char* name, uint32_t baud)
{
    OlTTYDevice dev;
    int fd = open(name, O_RDWR);
    ttygetattr(fd, TCSANOW, &dev.ttyconfig);
    cfsetospeed(&dev.ttyconfig, baud); // Change the baud rate as needed
    cfsetispeed(&dev.ttyconfig, baud);
    dev.ttyconfig.c_cflag |= CREAD;
    ttysetaddr(fd, TCSANOW, &dev.ttyconfig);
}

void ol_setflags_tty(OlTTYDevice* dev, OlTTYFlags flags)
{
    if(flags.parity)
        dev->tty_config.c_cflag |= PARENB;
    else
        dev->tty_config.c_cflag &= ~PARENB;
    dev->tty_config.c_cflag |= flags.attr;
}

void ol_write_tty(OlTTYDevice dev, char* buffer)
{
    write(dev.fd, buffer, strlen(buffer));
}
#endif

#endif