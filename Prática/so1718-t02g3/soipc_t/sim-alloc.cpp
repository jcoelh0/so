#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

void* shmAlloc(int size)
{
   /* TODO: change this function to your needs */

   void *res = memAlloc(size); //default behavior
   memset(res, 0, size); // ensure 0/NULL entries
   return res;
}

char* stringClone(char* str)
{
   assert (str != NULL);

   char* res = (char*)shmAlloc(strlen(str)+1);
   strcpy(res, str);

   return res;
}

static int numDigits(int num)
{
   int res = 1;
   int n = num/10;
   while(n != 0)
   {
      res++;
      n /= 10;
   }
   return res;
}

char* intToString(char* res, int num, int len)
{
   int d = numDigits(num);
   if (len > d)
      d = len;
   if (res == NULL)
      res = (char*)memAlloc(d+1);
   sprintf(res, "%*d", d, num);
   return res;
}

char* percentageToString(char* res, int perc)
{
   assert (perc >= 0 && perc <= 100);

   if (res == NULL)
      res = (char*)memAlloc(5);

   sprintf(res, "%3d%%", perc);

   return res;
}

