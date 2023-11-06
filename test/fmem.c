#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <windows.h>

typedef FILE* OlStream;
typedef struct
{
    uint8_t* ptr;
    size_t size;
    void* lock;
} OlBuffer;

OlStream ol_memstream(OlBuffer buf)
{
#ifdef _WIN32
    if(buf.ptr == NULL)
        return tmpfile();

    OlStream fp;
    char name[200], path[300];
    GetTempPathA(sizeof(name), name);
    GetTempFileNameA(name, "MemTF_", 0, path);
    HANDLE handle = CreateFileA(path,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                CREATE_NEW,
                FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                NULL);
    int fd = _open_osfhandle((intptr_t) handle, _O_RDWR);
    fp = _fdopen(fd, "w+");
    if(!fp) { _close(fd); return fp; }
    fwrite(buf.ptr, buf.size, 1, fp);
    rewind(fp); 
    return fp;
#elif defined(__linux__) || defined(__unix__)
    return fmemopen(buf.ptr, buf.size, mode);
#endif
}

int main()
{
    OlBuffer buf = {.ptr = malloc(10), .size = 10};
    memset(buf.ptr, 'A', 10);
    OlStream str = ol_memstream(buf);
    printf("Byte at pos 1 = %c", fgetc(str));
}