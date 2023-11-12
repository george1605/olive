#include <stdio.h>
#include <malloc.h>
#include "gfx.c"
#define OL_QUEUE_SIZE   0x20
#define OL_EVCLEAR      0x0 // or fill
#define OL_EVMEMCHANGED 0x1 // if front buffer is changed this event will be triggered
#define OL_EVSENDSND    0x2

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

// queue.pop() and elem.processor(elem)
void ol_process_event(OlMsgQueue* queue)
{
    OlMsg msg = queue->ptr[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
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
int ol_bsd_newqueue()
{
    kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
    }
    return kq;
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
}
*/