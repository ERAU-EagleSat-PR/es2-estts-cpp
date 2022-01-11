//
// Created by Hayden Roszell on 1/4/22.
//

#include <cstdio>
#include <cstdlib> /* for malloc, free, NULL */
#include "stack.h"

using namespace stack_ns;

/*
 * Function: getNewLink
 * Purpose: allocates new link with malloc and initializes value with variable
 * Returns: new link
 */
Link* stack::getNewLink(void * value, Link * next) {
    Link* newLink = (Link*)malloc(sizeof(Link));
    newLink->data = value;
    newLink->next = next;
    return newLink;
}

/*
 * Function: insert
 * Purpose: Insert value to linked list passed by fillList function
 * Returns: head of linked list
 */
void stack::push(void * data) {
    Link* new_link = getNewLink(data, nullptr); /* Allocates new link with new value */

    if (head == nullptr) {  /* Add new link to list if list is blank */
        head = new_link;
    }
    else {
        // Initialize data into temp data field
        new_link->data = data;

        // Put top pointer reference into temp link
        new_link->next = head;

        // Make temp as top of Stack
        head = new_link;
    }
}

/*
 * Function: delete
 * Purpose: Deletes specified value from linked list
 */
void * stack::pop() {
    Link* pop_ptr = head;

    if (pop_ptr == nullptr)
        return nullptr;

    head = head->next;
    pop_ptr->next = nullptr;
    void * data = pop_ptr->data;
    free(pop_ptr);

    return data;
}

stack::~stack() {
    while (head != nullptr) {
        auto temp = head;
        head = head->next;
        free(temp);
    }
}
