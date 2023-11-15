#include <stdio.h>
#include <malloc.h>
#include "gfx.c"
#include "snd.c"
#define OL_QUEUE_SIZE   0x20
#define OL_EVCLEAR      0x0 // or fill
#define OL_EVMEMCHANGED 0x1 // if front buffer is changed this event will be triggered
#define OL_EVSENDSND    0x2
#define OL_EVSDLCREATE  0x4
#ifdef _USE_SDL_
#include "../sdl/ol.h"
#endif

typedef struct _Msg
{
    int flags;
    void* data;
    void* target;
    void(*processor)(struct _Msg);
} OlMsg;

typedef struct
{
    OlMsg* ptr;
    int capacity;    // Maximum capacity of the queue
    int size;        // Current number of events in the queue
    int front;       // Front of the queue
    int rear;
    int error; // sets error if queue full or null ptr etc.
} OlMsgQueue;

OlMsgQueue* ol_new_queue(int capacity)
{
    OlMsgQueue* queue = (OlMsgQueue*)malloc(sizeof(OlMsgQueue));
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = capacity - 1;
    queue->ptr = (OlMsg*)malloc(queue->capacity * sizeof(OlMsg));
    return queue;
}

int ol_queue_empty(OlMsgQueue* queue) {
    return (queue->size == 0);
}

// Check if the event queue is full
int ol_queue_full(OlMsgQueue* queue) {
    return (queue->size == queue->capacity);
}

void ol_free_queue(OlMsgQueue* queue)
{
    if(queue == NULL) return;
    free(queue->ptr);
    free(queue);
}

/*
 * The default event handling function. If event.processor == NULL, this function is called
 */
void ol_handle_event(OlMsg event)
{
    switch(event.flags)
    {
        case OL_EVCLEAR:
            if(event.target == NULL || event.data == NULL) { break; }
            ol_fill(*(OlWindow*)(event.target), *(uint32_t*)(event.data));
            break;
        case OL_EVSENDSND:
            ol_play_buffer(*(OlSoundBuffer*)(event.data));
            break;
#ifdef _USE_SDL_
        // TO ADD SDLCREATE HANDLER!
        case OL_SDLCREATE:
            break;
#endif
        default:
            printf("Unknown event: %i", event.flags);
            break;
    }
}

// queue.pop() and elem.processor(elem)
void ol_process_event(OlMsgQueue* queue)
{
    OlMsg msg = queue->ptr[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    if(msg.processor == NULL)
        ol_handle_event(msg);
    else
        msg.processor(msg);
}

void ol_enqueue(OlMsgQueue* queue, void (*function)(OlMsg), void* data, void* target) 
{
    if (ol_queue_full(queue)) {
        queue->error = 1;
        return;
    }

    queue->rear = (queue->rear + 1) % queue->capacity; // is it circular? nope, look at the 'if' above
    queue->ptr[queue->rear].processor = function;
    queue->ptr[queue->rear].data = data;
    queue->ptr[queue->rear].target = target;
    queue->size++;
}

void ol_enqueue_msg(OlMsgQueue* queue, OlMsg msg)
{
    if (ol_queue_full(queue)) {
        queue->error = 1;
        return;
    }
}

void ol_def_handler(OlMsg msg)
{
    OlWindow* win = (OlWindow*)msg.target;
    printf("Event target is window with: %ix%i", win->w, win->h);
}

#ifdef __FreeBSD__
#include <sys/event.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    struct kevent* listen;
    struct kevent* triggered;
} OlBsdQueue;

void ol_bsd_checkev(struct kevent ev)
{
    if(ev.flags & EV_ERROR)
    {
            errx(EXIT_FAILURE, "Event error: %s", strerror(event.data));
    }
}

int ol_bsd_newqueue()
{
    kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
    }
    return kq;
}

// Monitor events from filedesc
void ol_bsd_setqueue(int fd, int flags, int kq)
{
    struct kevent event;
    if(flags == 0) flags = EV_ADD | EV_CLEAR;
    EV_SET(&event, fd, EVFILT_VNODE, flags, NOTE_WRITE,
	       0, NULL);
    kevent(kq, &event, 1, NULL, 0,	NULL);
}

OlBsdQueue ol_bsd_initqueue(size_t numEvents)
{
    OlBsdQueue q;
    q.listen = malloc(numEvents * sizeof(struct kevent));
    q.triggered = malloc(numEvents * sizeof(struct kevent)); // they are different
    return q;
}

struct event ol_bsd_nextev(int kq)
{
    struct event tevent;
    kevent(kq,	NULL, 0, &tevent, 1, NULL);
    return tevent;
}

#endif

/*
Exmaple code below

int main()
{
    ol_setup(200, 200);
    OlWindow win = ol_new_win();
    OlMsgQueue* queue = ol_new_queue(OL_QUEUE_SIZE);
    queue->error = 0;
    ol_enqueue(queue, ol_def_handler, NULL, &win);
    while(!ol_queue_empty(queue))
    {
        if(queue->error)
            printf("Got error");
        ol_process_event(queue);
    }
    ol_free_queue(queue);
    ol_save_ppm("main.ppm", win);
    free(win.front);
}
*/