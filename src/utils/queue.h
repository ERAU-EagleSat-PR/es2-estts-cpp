//
// Created by Hayden Roszell on 1/4/22.
//

#ifndef ESTTS_QUEUE_H
#define ESTTS_QUEUE_H

#define QUEUE_MAX_LENGTH 5

namespace queue_ns {
    typedef struct {
        void ** queueData;
        int front, rear;
        int size;
    } Queue;
}

class queue {
private:
    queue_ns::Queue * the_queue;
public:
    void insert(void * value);
    queue();
    void * remove();
};


#endif //ESTTS_QUEUE_H
