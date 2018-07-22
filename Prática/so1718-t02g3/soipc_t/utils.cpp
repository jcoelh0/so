#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sim-alloc.h"
#include "utils.h"

void* memAlloc(int size)
{
   void* result = malloc(size);
   check_alloc(result);
   return result;
}

int randomBoolean(int trueProb)
{
   double r = (double)rand()/(double)RAND_MAX; // [0;RAND_MAX]

   return r < (double)trueProb/100.0;
}

int randomInt(int min, int max)
{
   double r = (double)rand()/(double)RAND_MAX; // [0;RAND_MAX]

   return min + (int)(r*(max-min)+0.5);
}

char* randomString(char** list, int* used, int length)
{
   assert (list != NULL);
   assert (used != NULL);

   char* res = NULL;
   do
   {
      int idx = randomInt(0, length-1);
      if (!used[idx])
      {
         res = list[idx];
         used[idx] = 1;
      }
   }
   while(res == NULL);

   return res;
}

void clearConsole()
{
   printf("\u001B[2J");
}

void moveCursor(int line,int column)
{
   assert (column >= 0 && line >= 0);

   printf("\u001B[%d;%df", 1+line, 1+column);
}

int stringListLength(char** list)
{
   assert (list != NULL);

   int res;
   for(res = 0; list[res] != NULL; res++)
      ;
   return res;
}

