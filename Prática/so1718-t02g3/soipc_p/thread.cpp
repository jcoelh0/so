#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "thread.h"

#ifdef EXCEPTION_POLICY
#define check_error(status) \
   if (status != 0) \
      throw status
#else
#define check_error(status) \
   if (status != 0) \
      do { \
         fprintf (stderr, "%s at \"%s\":%d: %s\n", \
                  __FUNCTION__ , __FILE__, __LINE__, strerror (status)); \
         *((int*)0) = 0; \
         abort (); \
      } while (0)
#endif

int thread_equal(pthread_t t1, pthread_t t2)
{
   return pthread_equal(t1, t2);
}

void thread_create(pthread_t* t, pthread_attr_t* attr, void *(*thread_main)(void*), void* arg)
{
   assert(t != NULL);
   assert(thread_main != NULL);

   int status = pthread_create(t, attr, thread_main, arg);
   check_error(status);
}

void thread_detach(pthread_t thread)
{
   int status = pthread_detach(thread);
   check_error(status);
}

void thread_exit(void *retval)
{
   pthread_exit(retval);
}

pthread_t thread_self()
{
   return pthread_self();
}

void thread_join(pthread_t t, void** result)
{
   int status = pthread_join(t, result);
   check_error(status);
}


void mutex_init(pthread_mutex_t* pmtx, pthread_mutexattr_t* attr)
{
   assert(pmtx != NULL);

   int status = pthread_mutex_init(pmtx, attr);
   check_error(status);
}

void mutex_destroy(pthread_mutex_t* pmtx)
{
   assert(pmtx != NULL);

   int status = pthread_mutex_destroy(pmtx);
   check_error(status);
}

void mutex_lock(pthread_mutex_t* pmtx)
{
   assert(pmtx != NULL);

   int status = pthread_mutex_lock(pmtx);
   check_error(status);
}

// returns 0 on lock failure
int mutex_trylock(pthread_mutex_t* pmtx)
{
   assert(pmtx != NULL);

   int status = pthread_mutex_trylock(pmtx);
   if (status != 0 && status != EBUSY)
      check_error(status);

   return status != EBUSY;
}

void mutex_unlock(pthread_mutex_t* pmtx)
{
   assert(pmtx != NULL);

   int status = pthread_mutex_unlock(pmtx);
   check_error(status);
}

void cond_init(pthread_cond_t* pcvar, pthread_condattr_t* attr)
{
   assert(pcvar != NULL);

   int status = pthread_cond_init(pcvar, attr);
   check_error(status);
}

void cond_destroy(pthread_cond_t* pcvar)
{
   assert(pcvar != NULL);

   int status = pthread_cond_destroy(pcvar);
   check_error(status);
}

void cond_wait(pthread_cond_t* pcvar, pthread_mutex_t* pmtx)
{
   assert(pcvar != NULL);
   assert(pmtx != NULL);

   int status = pthread_cond_wait (pcvar, pmtx);
   check_error(status);
}

void cond_signal(pthread_cond_t* pcvar)
{
   assert(pcvar != NULL);

   int status = pthread_cond_signal (pcvar);
   check_error(status);
}

void cond_broadcast(pthread_cond_t* pcvar)
{
   assert(pcvar != NULL);

   int status = pthread_cond_broadcast (pcvar);
   check_error(status);
}

void mutexattr_init(pthread_mutexattr_t *attr)
{
   assert(attr != NULL);

   int status = pthread_mutexattr_init (attr);
   check_error(status);
}

void mutexattr_destroy(pthread_mutexattr_t *attr)
{
   assert(attr != NULL);

   int status = pthread_mutexattr_destroy (attr);
   check_error(status);
}

// type: PTHREAD_MUTEX_NORMAL PTHREAD_MUTEX_ERRORCHECK PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_DEFAULT
void mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
   assert(attr != NULL);

   int status = pthread_mutexattr_settype (attr, type);
   check_error(status);
}


#ifdef NOT_INCLUDED_IN_ACTIVE_CODE
void create_key(pthread_key_t* pkey) // once execution (in main or using pthread_once)
{
   assert(pkey != NULL);

   int status = pthread_key_create (pkey, NULL);
   check_error(status);
}

void setspecific(pthread_key_t* pkey, void* value)
{
   assert(pkey != NULL);
   assert(value != NULL);

   int status = pthread_setspecific (*pkey, value);
   check_error(status);
}

void* getspecific(pthread_key_t* pkey)
{
   assert(pkey != NULL);

   return pthread_getspecific (*pkey);
}

int randomValue(int min, int max)
{
   assert(min >= 0 && max >= 0);

   unsigned int* pseed = (unsigned int*)getspecific(&rand_seed_key);
   assert(pseed != NULL);
   return min + (int)((double)(max-min)*(double)rand_r(pseed)/(double)RAND_MAX);
}

void randomPause(int min, int max) // msec
{
   usleep(randomValue(min, max)*1000);
}
#endif
