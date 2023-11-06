#include <SDL2/SDL.h>
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

void ol_android_win()
{

}

#elif defined(__APPLE__)

#endif