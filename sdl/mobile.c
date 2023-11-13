#include "include/SDL2/SDL.h"
#include "../include/gfx.c"
#include "../include/errors.h"

typedef struct
{
    uint16_t width, height;
} OlSize;

#ifdef __ANDROID__
#include <jni.h> // a must
#include <android/multinetwork.h>

/**
 * Grabs a native window using SDL and JNI functions
*/
ANativeWindow* ol_getandroid_win()
{
    JNIEnv* env = SDL_AndroidGetJNIEnv();
    ANativeActivity* f = SDL_AndroidGetActivity();
    return ANativeWindow_fromSurface(env, f->clazz);
}

ANativeWindow* ol_androidwin_from(SDL_Window* win)
{
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(win, &wmInfo);
    return wmInfo.info.android.window;
}

/*
 * Grabs the (width, height) dimensions of the Android Window
 */
OlSize ol_android_dim(ANativeWindow* win)
{
    OlSize size;
    size.width = ANativeWindow_getWidth(win);
    size.height = ANativeWindow_getHeight(win);
    return size;
}

/*
 * Creates an Olive Window object from Native Window
 */
OlWindow ol_android_canvas(ANativeWindow* win)
{
    ANativeWindow_Buffer buffer;
    OlSize size = ol_android_dim(win);
    if(ANativeWindow_lock(win, &buffer, NULL) < 0)
    {
        ol_error = OL_EANDRWINLOCK;
        return (OlWindow){.front = NULL};
    }
    OlWindow win;
    win.front = (uint32_t*)buffer.bits;
    win.width = size.width;
    win.height = size.height;
    return win;
}

void ol_android_unlock(ANativeWindow* win)
{
    ANativeWindow_unlockAndPost(win);
}

#elif defined(__APPLE__)
// TO DO: Add APPLE specific functions
#endif