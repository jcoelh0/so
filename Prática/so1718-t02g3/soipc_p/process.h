#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>

//#define EXCEPTION_POLICY
//#define EXIT_POLICY // DEFAULT

// process
//
pid_t pfork(void);
pid_t pwait(int *status);
pid_t pwaitpid(pid_t pid, int *status, int options);

// System V - shared memory

int pshmget(key_t key, size_t size, int shmflg);
int pshmctl(int shmid, int cmd, struct shmid_ds *buf);
void *pshmat(int shmid, const void *shmaddr, int shmflg);
void pshmdt(const void *shmaddr);

// System V - semaphores

int psemget(key_t key, int nsems, int semflg);
int psemctl(int semid, int semnum, int cmd, ...);
void psemop(int semid, struct sembuf *sops, size_t nsops);
void psem_up(int semid, short unsigned int index);
void psem_down(int semid, short unsigned int index);

// POSIX semaphores

// named:
sem_t *psem_open(const char *name, int oflag, ...);
void psem_close(sem_t *sem);
void psem_unlink(const char *name);

// unnamed:
void psem_init(sem_t *sem, int pshared, unsigned int value);
void psem_destroy(sem_t *sem);

// named and unnamed:
void psem_wait(sem_t *sem);

/**
 *  \brief Wrapper for sem_trywait.
 *
 *  \param sem memory address of sem_t variable
 *  \return true on success, false is semaphore is zero
 **/
int psem_trywait(sem_t *sem);

/**
 *  \brief Wrapper for sem_trywait.
 *
 *  \param sem memory address of sem_t variable
 *  \param abs_timeout timeout specification
 *  \return true on success, false if timeout has expired before being able to decrement the semaphore
 **/
int psem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

void psem_post(sem_t *sem);

#endif
