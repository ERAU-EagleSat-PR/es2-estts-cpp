//
// Created by Hayden Roszell on 1/4/22.
//

#include "queue.h"

using namespace queue_ns;

queue::queue() {
    the_queue->front = 0;
    the_queue->rear = 0;
    the_queue->size = 0;
}

void queue::insert(void * value) {
    int temp = the_queue->rear;

    if (the_queue->size < QUEUE_MAX_LENGTH) {
        if (the_queue->size == 0) {
            the_queue->rear = the_queue->front = 0;
            the_queue->queueData[0] = value;
            ++the_queue->size;
        }
        else if (++temp == QUEUE_MAX_LENGTH) {
            the_queue->rear = 0;
            the_queue->queueData[0] = value;
            ++the_queue->size;
        }
        else {
            the_queue->queueData[the_queue->rear + 1] = value;
            ++the_queue->rear;
            ++the_queue->size;
        }
    }
}

void * queue::remove() {
    if (the_queue->size == 0 ) {
        return nullptr;
    }
    else {
        the_queue->front++;
        the_queue->size--;
    }
    return the_queue->queueData[the_queue->front];
}