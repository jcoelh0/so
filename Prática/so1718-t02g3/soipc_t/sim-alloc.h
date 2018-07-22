/**
 *  \brief Simulation memory alocator
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef SIM_ALLOC_H
#define SIM_ALLOC_H

#include <alloca.h>
#include <string.h>
#define concat_string_in_stack(str1,str2) \
   ({ \
      char* __res__ = (char*)alloca(strlen(str1)+strlen(str2)+1); \
      strcpy(__res__, str1); \
      strcat(__res__, str2); \
      __res__; \
   })

void* shmAlloc(int size);
char* stringClone(char* str);
char* intToString(char* res, int num, int len);
char* percentageToString(char* res, int perc);

#endif
