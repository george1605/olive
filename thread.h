#ifndef __THREADS__
#define __THREADS__
#include <stdint.h>

typedef struct {
    void* handle;
} OlThread;

typedef struct {
    OlThread* threads;
    size_t number;
} OlTManager;

#ifdef _WIN32
#include <windows.h>
typedef LPTHREAD_START_ROUTINE threadf;

OlThread ol_new_thread(threadf threadFunc, void* args)
{
    OlThread i;
    i.handle = CreateThread(NULL, 0, threadFunc, args, 0, NULL);
    return i;
}

void ol_wait_threads(OlTManager p)
{
    int i;
    for(i = 0;i <= p.number;i++)
        WaitForSingleObject(p.threads[i].handle, INFINITE);
}

void ol_close_thread(OlThread thread)
{
    TerminateThread(thread.handle, 0);
}

#else // use pthreads
#include <unistd.h>
#include <pthread.h>

OlThread ol_new_thread(void(*f)(void*), void* data)
{
    pthread_t thread;
    pthread_create(&thread, NULL, f, data);
    OlThread t;
    t.handle = (void*)thread;
    return t;
}

void ol_join_threads(OlTManager p)
{
    int i;
    for(i = 0;i <= p.number;i++)
        pthread_join(p.threads[i].handle, NULL);
}

void ol_close_thread(OlThread thread)
{
    pthread_cancel(thread.handle);
}
#endif

#endif