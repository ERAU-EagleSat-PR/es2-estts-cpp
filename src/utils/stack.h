//
// Created by Hayden Roszell on 1/4/22.
//

#ifndef ESTTS_STACK_H
#define ESTTS_STACK_H

/*
 * Struct: Link
 * Purpose: Outline for linked list content
 */
namespace stack_ns {
    typedef struct link {
        struct link* next;
        void * data;
    } Link;
}


class stack {
private:
    stack_ns::Link * head;
    static stack_ns::Link* getNewLink(void * value, stack_ns::Link* next);
protected:

public:
    stack() { head = nullptr; }
    void push(void * data);
    void * pop();
    ~stack();
};


#endif //ESTTS_STACK_H
