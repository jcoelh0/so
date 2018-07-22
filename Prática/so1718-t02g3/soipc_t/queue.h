/**
 *  \brief A simple linked list queue (with some extra array like operation).
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct _Item_
{
   struct _Item_* next;
   void* elem;
} Item;

typedef struct _Queue_
{
   int alloc;
   Item* head;
   Item* tail;
   int size;
} Queue;

Queue* newQueue(Queue* queue);
Queue* destroyQueue(Queue* queue);
void inQueue(Queue* queue, void* elem);
void* outQueue(Queue* queue);
int emptyQueue(Queue* queue);
int sizeQueue(Queue* queue);
// array-like operations
void* getQueue(Queue* queue, int idx);
void* removeQueue(Queue* queue, int idx);

#endif
