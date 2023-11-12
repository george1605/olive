/*
*   IO.C Source File
*   Contains functions regarding file information, buffers, devices and TTY/COM(USB) communication,
*   pipes and more IO related functions
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
#include <io.h>
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

typedef struct _OlIOContext {
    OlTask* tasks;
    size_t number, size;
    int(*check)(struct _OlIOContext*);
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
    void* handles[2];
    char* name; // NULL for unnamed Pipes
} OlPipe;

typedef struct {
    char* name;
    void* handle;
    uint8_t* buffer;
    void(*read)(uint8_t*, size_t);
    void(*write)(uint8_t*, size_t);
} OlBufDevice; // buffered device

// un-named pipes
OlPipe ol_new_pipe(FILE* file1, FILE* file2)
{
    OlPipe pipe;
    pipe.name = NULL;
    pipe.handles[0] = file1;
    pipe.handles[1] = file2;
#ifdef __linux__
    int fds[2];
    fds[0] = fileno(file1);
    fds[1] = fileno(file2);
    pipe(fds);
#elif defined(_WIN32)
    HANDLE read = (HANDLE)_get_osfhandle(_fileno(file1));
    HANDLE write = (HANDLE)_get_osfhandle(_fileno(file2));
    CreatePipe(read, write, NULL, 512);
#endif
    return pipe;
}

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
        dev.read(&dev, buf->ptr + buf->crs, buf->size);
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

// memory page
typedef struct _OlPage {
    void* ptr;
    size_t size;
    struct _OlPage* next;
} OlPage;

// gets all the pages -> buffer
OlBuffer ol_page_tobuf(OlPage* pages)
{
    OlBuffer buf;
    OlPage* head = pages;
    while(pages->next)
    {
        buf.size += pages->size;
        pages = pages->next;
    }
    buf.ptr = malloc(buf.size + 1);
    while(head)
    {
        memcpy(buf.ptr, head->ptr, head->size);
        head = head->next;
        free(buf.ptr);
    }
    buf.crs = 0;
    return buf;
}

// it reads pages from file till the end
void ol_read_pages(OlPage* page, FILE* fp)
{
    size_t page_sz = page->size;
    page->next = malloc(sizeof(OlPage));
    page = page->next;
    page->ptr = malloc(page_sz);
    while(fread(page->ptr, 1, page_sz, fp))
    {
        page->size = page_sz;
        page->next = malloc(sizeof(OlPage));
        page = page->next;
        page->ptr = malloc(page_sz);
    }
}

void ol_buf_write(OlBuffer buf, char* x, FILE* fp)
{
    size_t sz = strlen(x);
    if(buf.crs + sz > buf.size)
    {
        size_t p = buf.size - buf.crs;
        size_t r = buf.crs + sz - buf.size;
        memcpy(buf.ptr + buf.crs, x, p);
        ol_bufflush(&buf, fp);
        memcpy(buf.ptr, x, r);
    } else {
        memcpy(buf.ptr + buf.crs, x, sz);
    }    
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

typedef void(*OlModFunc)(OlModule*);

OlModule ol_read_module(char* fname)
{
    OlModule mod;
    #ifdef _WIN32
        HANDLE hnd = LoadLibraryA(fname);
        mod.init = (OlModFunc)GetProcAddress(hnd, "ol_init");
        mod.exit = (OlModFunc)GetProcAddress(hnd, "ol_exit");
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

static void __ol_tmpread(OlDevice* dev, uint8_t* buf, size_t size)
{
    fread(buf, 1, size, dev->handle);
}

static void __ol_tmpwrite(OlDevice* dev, uint8_t* buf, size_t size)
{
    fwrite(buf, 1, size, dev->handle);
}

OlDevice ol_temp_dev()
{
    OlDevice dev;
    dev.name = "tmp";
    dev.handle = tmpfile();
    dev.read = __ol_tmpread;
    dev.write = __ol_tmpwrite;
    return dev;
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

typedef struct
{
    OlDevice dev;
    OlBuffer buf;
} OlIOInfo;

static void __ol_asread(void* f)
{
    OlDevice dev = ((OlIOInfo*)f)->dev;
    OlBuffer buf = ((OlIOInfo*)f)->buf;
    dev.read(&dev, buf.ptr, buf.size);
}

static void __ol_aswrite(void* f)
{
    OlDevice dev = ((OlIOInfo*)f)->dev;
    OlBuffer buf = ((OlIOInfo*)f)->buf;
    dev.write(&dev, buf.ptr, buf.size);
}

void ol_asread_dev(OlDevice dev, OlBuffer buf)
{
    OlIOInfo info = {.dev = dev, .buf = buf};
    OlThread thread = ol_new_thread((threadf)__ol_asread, &info);
    ol_wait_thread(thread);
}

void ol_aswrite_dev(OlDevice dev, OlBuffer buf)
{
    OlIOInfo info = {.dev = dev, .buf = buf};
    OlThread thread = ol_new_thread((threadf)__ol_aswrite, &info);
    ol_wait_thread(thread);
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
    buf[9] = '\0';
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
    WriteFile(hnd, buf.ptr, buf.size, (LPDWORD)&bytesWritten, NULL);
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

// Registers an USB Device using the Olive Device Interface
void ol_register_usb(int number, OlCOMInfo info)
{
    OlDevice dev;
    dev.name = "usb";
    dev.handle = ol_open_com(number);
    dev.read = __ol_usbread;
    dev.write = __ol_usbwrite;
    ol_register_dev(dev);
}

// finds the device and closes the handle - must be done at the end of the program
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

static char* __ol_ttypath = NULL;

OlTTYDevice ol_open_tty(int number)
{
    OlTTYDevice dev;
    if(!__ol_ttypath)
        ol_set_ttypath(NULL);
    char* name = strdup(__ol_ttypath);
    name[strlen(name) - 1] = ((number % 10) + '0')
    dev.fd = open(name, O_RDWR);
    return dev;
}

void ol_set_ttypath(char* path)
{
    if(!path)
        __ol_ttypath = "/dev/ttyUSB*";
    else
        __ol_ttypath = path; // like /dev/ttyS0 or smth
}

void ol_setflags_tty(OlTTYDevice* dev, OlTTYFlags flags)
{
    ttygetattr(fd, TCSANOW, &(dev.config));
    cfsetospeed(&dev.ttyconfig, flags.baud); // Change the baud rate as needed
    cfsetispeed(&dev.ttyconfig, flags.baud);
    if(flags.parity)
        dev->tty_config.c_cflag |= PARENB;
    else
        dev->tty_config.c_cflag &= ~PARENB;
    dev->tty_config.c_cflag |= flags.attr;
    ttysetattr(fd, TCSANOW, &(dev.config));
}

void ol_write_tty(OlTTYDevice dev, char* buffer)
{
    write(dev.fd, buffer, strlen(buffer));
}

void __ol_write_tty(OlTTYDevice dev, uint8_t* buf, size_t size)
{
    write(dev.fd, buf, size);
}

void __ol_read_tty(OlTTYDevice dev, uint8_t* buf, size_t size)
{
    read(dev.fd, buf, size);
}

void ol_register_usb(int number, OlTTYFlags flags)
{
    OlTTYDevice tty = ol_open_tty("/dev/tty0");
    ol_setflags_tty(&tty, flags);
    OlDevice dev;
    dev.name = "usb";
    dev.handle = tty.fd;
    dev.read = __ol_read_tty;
    dev.write = __ol_write_tty;
    ol_register_dev(dev);
}
#endif

#endif