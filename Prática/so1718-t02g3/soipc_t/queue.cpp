#include <stdlib.h>
#include <assert.h>
#include "utils.h"
#include "queue.h"

Queue* newQueue(Queue* queue)
{
   if (queue == NULL)
   {
      queue = (Queue*)memAlloc(sizeof(Queue));
      queue->alloc = 1;
   }
   else
      queue->alloc = 0;
   queue->head = queue->tail = NULL;
   queue->size = 0;
   return queue;
}

Queue* destroyQueue(Queue* queue)
{
   assert (queue != NULL);

   while(queue->head != NULL)
      outQueue(queue);
   if (queue->alloc)
   {
      free(queue);
      queue = NULL;
   }
   return queue;
}

void inQueue(Queue* queue, void* elem)
{
   assert (queue != NULL);
   assert (elem != NULL);

   Item* item = (Item*)memAlloc(sizeof(Item));
   item->elem = elem;
   item->next = NULL;
   if (queue->tail != NULL)
      queue->tail->next = item;
   else
      queue->head = item;
   queue->tail = item;
   queue->size++;
}

void* outQueue(Queue* queue)
{
   assert (queue != NULL);
   assert (sizeQueue(queue) > 0);

   Item* itm = queue->head;
   void* res = queue->head->elem;
   queue->head = queue->head->next;
   if (queue->head == NULL)
      queue->tail = NULL;
   queue->size--;
   free(itm);
   return res;
}

int emptyQueue(Queue* queue)
{
   assert (queue != NULL);

   return queue->size == 0;
}

int sizeQueue(Queue* queue)
{
   assert (queue != NULL);

   return queue->size;
}

void* getQueue(Queue* queue, int idx)
{
   assert (queue != NULL);
   assert (idx >= 0 && idx < sizeQueue(queue));

   Item* itm = queue->head;
   for (int i = 0; i < idx; i++)
      itm = itm->next;
   return itm->elem;
}

void* removeQueue(Queue* queue, int idx)
{
   assert (queue != NULL);
   assert (idx >= 0 && idx < sizeQueue(queue));

   void* res;

   Item* itm = queue->head;
   Item* last = NULL;
   for (int i = 0; i < idx; i++)
   {
      last = itm;
      itm = itm->next;
   }
   res = itm->elem;
   if (last == NULL)
   {
      queue->head = queue->head->next;
      if (queue->head == NULL || queue->head->next == NULL)
         queue->tail = queue->head;
   }
   else
   {
      last->next = last->next->next;
      if (last->next == NULL)
         queue->tail = last;
   }
   queue->size--;
   return res;
}

