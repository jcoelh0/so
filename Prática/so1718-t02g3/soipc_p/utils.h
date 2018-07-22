/**
 *  \brief Useful common functions and macros.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

//#define EXCEPTION_POLICY
//#define EXIT_POLICY // DEFAULT

#ifdef EXCEPTION_POLICY
#define check_alloc(pnt) \
   if (pnt == NULL) \
      throw ENOMEM
#else
#define check_alloc(pnt) \
   if (pnt == NULL) \
      do { \
         fprintf (stderr, "%s at \"%s\":%d: %s\n", \
                  __FUNCTION__ , __FILE__, __LINE__, strerror (ENOMEM)); \
         *((int*)0) = 0; \
         abort (); \
      } while (0)
#endif

void* memAlloc(int size);
int randomBoolean(int trueProb);
int randomInt(int min, int max);
char* randomString(char** list, int* used, int length);
void clearConsole();
void moveCursor(int line,int column);
int stringListLength(char** list);

#endif
