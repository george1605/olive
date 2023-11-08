#ifndef __THREADS__
#define __THREADS__
#include <stdint.h>
#include "event.c"

typedef struct {
    void* handle;
} OlThread;

typedef struct {
    void* handle;
    OlMsgQueue* queue; 
} OlEvThread;

typedef struct {
    OlThread* threads;
    size_t number;
} OlTManager;

typedef struct {
    void* target;
    void* handle;
    int status;
} OlLock;

#ifdef _WIN32
#include <windows.h>
typedef LPTHREAD_START_ROUTINE threadf;

OlLock ol_new_lock()
{
    OlLock t;
    t.target = NULL;
    t.handle = CreateMutex(NULL, FALSE, NULL);
    t.status = FALSE;
    return t;
}

void ol_wait_lock(OlLock* l)
{
    l->status = WaitForSingleObject(l->handle, INFINITE);
}

void ol_delete_lock(OlLock l)
{
    CloseHandle(l.handle);
}

int ol_release_lock(OlLock l)
{
    if(!ReleaseMutex(l.handle))
        return -1;
    return 0;
}

OlThread ol_new_thread(threadf threadFunc, void* args)
{
    OlThread i;
    i.handle = CreateThread(NULL, 0, threadFunc, args, 0, NULL);
    return i;
}

void ol_wait_thread(OlThread thread)
{
    WaitForSingleObject(thread.handle, INFINITE);
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

typedef struct {
    void(*function)(void);
    int synchronous : 1;
    int completed : 1;
    int thread_complete : 1;
    int return_on_cancel : 1;
    OlLock lock;
} OlTask;

void ol_run_task(OlTask task)
{
    if(task.synchronous)
        task.function();
    else {
        OlThread th;
        th = ol_new_thread((threadf)task.function, NULL);
        ol_wait_thread(th);
    }
}

#endif