#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

//#define EXCEPTION_POLICY
//#define EXIT_POLICY // DEFAULT

// thread:
int thread_equal(pthread_t t1, pthread_t t2);
void thread_create(pthread_t* t, pthread_attr_t* attr, void *(*thread_main)(void*), void* arg);
void thread_detach(pthread_t thread);
void thread_exit(void *retval);
pthread_t thread_self();
void thread_join(pthread_t t, void** result);

// mutex:

void mutex_init(pthread_mutex_t* pmtx, pthread_mutexattr_t* attr);
void mutex_destroy(pthread_mutex_t* pmtx);
void mutex_lock(pthread_mutex_t* pmtx);
int mutex_trylock(pthread_mutex_t* pmtx); // returns 0 (false) on lock failure
void mutex_unlock(pthread_mutex_t* pmtx);

// cvar:
void cond_init(pthread_cond_t* pcvar, pthread_condattr_t* attr);
void cond_destroy(pthread_cond_t* pcvar);
void cond_wait(pthread_cond_t* pcvar, pthread_mutex_t* pmtx);
void cond_signal(pthread_cond_t* pcvar);
void cond_broadcast(pthread_cond_t* pcvar);

// mutex attributes:
void mutexattr_init(pthread_mutexattr_t *attr);
void mutexattr_destroy(pthread_mutexattr_t *attr);
void mutexattr_settype(pthread_mutexattr_t *attr, int type);

// thread specific data:
//void create_key(pthread_key_t* pkey); // once execution (in main or using pthread_once)
//void setspecific(pthread_key_t* pkey, void* value);
//void* getspecific(pthread_key_t* pkey);

#endif
