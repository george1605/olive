#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#endif

// maybe too much
#include "include/gfx.c"
#include "include/io.c"
#ifdef _USE_SOUND_
#include "include/snd.c"
#endif

#ifdef _USE_GDI_
#include "include/gdi.c"
#endif

#ifdef _USE_SDL_
#include "sdl/ol.c"
#include "sdl/sound.c"
#endif

#ifdef _USE_OPENGL_
#include "opengl/olivegl.h"
#include "opengl/oliveglut.h"
#endif

#ifdef _USE_NET_
#include "net-lib/main.c"
#endif

#ifdef _USE_X11_
#include "low/x11.h"
#endif
typedef FILE* OlStream;

// Making fmemopen() portable
OlStream ol_memstream(OlBuffer buf, char* mode)
{
#ifdef _WIN32
    if(buf.ptr == NULL)
        return tmpfile();

    OlStream fp;
    char path[200];
    HANDLE handle = CreateFileA(path,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                CREATE_NEW,
                FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                NULL);
    int fd = _open_osfhandle((intptr_t) handle, _O_RDWR);
    fp = _fdopen(fd, mode);
    if(!fp) _close(fd); 
    return fp;
#elif defined(__linux__) || defined(__unix__)
    return fmemopen(buf.ptr, buf.size, mode);
#endif
}

// a Platform struct that includes handles to core modules, part of OlApp
typedef struct 
{
    void* video_handle;
    void* audio_handle;
    void* net_handle;
    void* sys_handle; 
} OlPlatform;

typedef struct {
    int argc;
    char** argv;
} OlArgs;

typedef struct 
{
    OlArgs args;
    OlPlatform platform;
    void(*setup)(OlPlatform* p);
    void(*main)(OlPlatform, OlArgs);
} OlApp;

typedef struct
{
    char* path; // using getcwd()
    char* os;
} OlSysInfo;

void ol_setupvid_platform(OlPlatform* platform, int w, int h)
{
    if(platform->video_handle != NULL) return;
    OlWindow* win = (OlWindow*)malloc(sizeof(OlWindow));
    win->front = (u32*)malloc(4 * w * h);
    win->w = w, win->h = h;
    platform->video_handle = win;
}

void ol_getsys_platorm(OlPlatform* platform)
{
    OlSysInfo* info = malloc(sizeof(OlSysInfo));
#ifdef _WIN32
    info->os = "Windows";
#elif defined(__linux__)
    info->os = "Linux";
#elif defined(__APPLE__)
    info->os = "macOS";
#else
    info->os = "Unknown";
#endif
    platform->sys_handle = info;
}

void ol_freevid_platform(OlPlatform platform)
{
    OlWindow* win = (OlWindow*)platform.video_handle;
    if(win == NULL) return; 
    free(win->front);
    free(win);
}

void ol_run_app(OlApp app)
{   
    app.main(app.platform, app.args);
}

int ol_lib_exist(char* lname)
{
    #ifdef _WIN32
        return (LoadLibraryA(lname) != NULL);
    #elif defined(__linux__)
        void* handle = dlopen(lname, RTLD_NOW);
        if(handle == NULL)
            return 0;
        dlclose(handle);
        return 1;
    #else
        return 0;
    #endif
}

#ifdef _REDEF_MAIN_
void __main(OlPlatform platform, OlArgs args);
void __setup(OlPlatform* platform);

int main(int argc, char** argv)
{
    OlApp app;
    app.setup = __setup, app.main = __main;
    app.setup(&app.platform);
    app.args.argc = argc;
    app.args.argv = argv;
    ol_run_app(app);
    return 0;
}
#endif