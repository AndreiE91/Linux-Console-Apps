#include <stdio.h>      // for printf()
#include <pthread.h>    // for pthread_t, pthread_create(), pthread_join(), pthread_sigmask()
#include <signal.h>     // for signal(), SIGUSR1, sigset_t, sigemptyset(), sigaddset()

// The response to a received signal, as well as the functions that handle
// signals are common to all threads of a process: functions signal or sigaction

//  Defining the mask for a thread is done with the help of the
// function pthread_sigmask, and for the whole process with the function
// sigprocmask

void sigHandler(int sig) {
    printf("Thread %ld handles the signal\n", pthread_self());
}

void* thFunction(void* arg) {
    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGUSR1);
    pthread_sigmask(SIG_SETMASK, &sigmask, NULL);
    while(1); // Endless loop when running this example, very bad I don't really get the point of it
}

int main() {
    pthread_t th1;
    signal(SIGUSR1, sigHandler);
    pthread_create(&th1, NULL, thFunction, NULL);
    pthread_join(th1, NULL);

    return 0;
}

#include <pthread.h>

// Create a new, concurrent executing thread to the thread calling this function
int pthread_create ( // Returns 0 for success and >0 for error
pthread_t* idThread, // Memory address where identifier of newly created thread will be stored
const pthread_attr_t* threadAttributes, // Memory address of a structure containing values of attributes used when creating thread. NULL means default values are used
void* (*thread_routine)(void*), // Memory address of the function the thread will execute
void* arg); // Address where argument passed to said function is stored. Argument can have any desired structure

// Begins by running thread_routine function, either ends explicitly by calling pthread_exit or implicitly at the end of the function

// Reltionship-wise, all threads of a process are equal(thread-to-thread-wise)
// Exception is main thread, which ends ALL of its threads when terminated
// Main thread is the one executing the main() function
// Thus, main thread waits for all of its threads to finish before terminating

// Create a thread like this:
void* thFunction(void* arg) {
    int* val = (int*) arg;
    printf("Thread with argument %d\n", *val);
}
int main() {
    pthread_t th1;
    int arg1 = 1;
    pthread_create(&th1, NULL, thFunction, &arg1); // Each thread has unique ID assigned to it at creation, returned by this pthread_create function to the thread calling it
    // Can also obtain own id by pthread_self call
    pthread_join(th1, NULL);
}

// Use gcc -Wall -Werror progr.c –lpthread –o progr to compile


// Compare the ID values of two threads:
#include <pthread.h>
pthread_t pthread_self(void);
int pthread_equal( pthread_t thread1,
pthread_t thread2);

// gettid can be used on Linux to obtain OS assigned thread ID of said thread
#include <sys/types.h>
pid_t gettid(void);

// Usually threads terminate execution when returning from function they execute
// Another way is by calling pthread_exit
#include <pthread.h>
void pthread_exit(void *retval); //at the moment a thread ends, all its resources are freed if this is not called
// the information
// about the termination status of a thread is given by the value of the retval
// parameter of the function pthread_exit, in case this function was called, or
// by the returned value of the function executed by the thread.



// A thread can be ended only by another thread of the same process. The
// permission to terminate a thread from another thread is given by another of
// the thread's attributes, which is the one named cancelability state.

int pthread_cancel(pthread_t thread);

int pthread_setcancelstate(int state, int *oldstate);
// PTHREAD_CANCEL_ENABLE: the thread can be stopped;
// PTHREAD_CANCEL_DISABLE: the thread cannot be stopped

// it can chose between immediately ending its execution or waiting
// till it reaches a so called cancel point
int pthread_setcanceltype(int type, int *oldtype);
// PTHREAD_CANCEL_ASYNCHRONOUS: immediate cancel;
// PTHREAD_CANCEL_DEFERRED: delayed cancel.

// Cancel points are those points in the execution of a thread where the system
// checks if there are cancel requests and the corresponding operations are
// carried out : pthread_join, pthread_cond_wait, pthread_cond_timedwait,
// pthread_testcancel, sem_wait, sigwait, sleep and any function that puts the   /// Other POSIX functions aren’t considered cancel points!
// thread in waiting

// ONLY pthread_testcancel is explicitly set by the programmer, others are automatic cancel points

void pthread_testcancel(void);
// Cancel state: PTHREAD_CANCEL_ENABLE
// Cancel type: PTHREAD_CANCEL_DEFFERRED

// Terminating a thread from another thread has to be made with a lot of
// caution because a couple of serious problems may be generated. This is
// because in the moment the thread receives a cancel request, the thread can
// contain global data that is in an inconsistent state or hold resources (locks,
// semaphores etc.) needed by other threads and which must be released before
// the thread ends

// This can be done by
// setting the cancel type to PTHREAD_DEFERRED and by specifying
// certain functions that have to be executed in the moment the thread ends,
// either by calling pthread_exit, or by calling pthread_cancel by another
// thread


// pthread_cleanup_push
// pthread_cleanup_pop
// LIFO
void pthread_cleanup_push(
void (*routine)(void*), // The function to be executed at the end of the thread
void* arg); // The argument passed to this function
void pthread_cleanup_pop(int execute); // whether the function popped from the stack has to be executed or not

// ALWAYS USED AS PAIRS
// pthread_cleanup_push
// uses the open bracket ‘{', while pthread_cleanup_pop uses the closing
// bracket '}'
typedef struct m {
    int size;
    void* pMem;
} MEM;
void allocate_mem(void* arg) {
    MEM* p = (MEM*) arg;
    p>pMem = malloc(p>size);
}
void release_mem(void* arg) {
    MEM* p = (MEM*) arg;
    if (p>pMem)
    free(p>pMem);
}
void* thFunction(void* arg){
    int oldType;
    MEM thMem;
    pthread_setcanceltype(
    PTHREAD_CANCEL_DEFERRED, &oldType);
    thMem.size = 100;
    thMem.pMem = NULL;
    pthread_cleanup_push(
    release_mem, (void *) &thMem);
    allocate_mem(&thMem);
    /* do some work with the memory*/
    pthread_cleanup_pop(1);
    pthread_setcanceltype(oldType, NULL);
}

// blocking a thread to wait until another thread ends its execution:
int pthread_join(pthread_t th, void **thread_return);

// Information about the termination of the thread for which it waits is
// organized in a structure whose address will be the one in the thread_return
// parameter, if its value is different from NULL

// Calling this function will result in blocking the execution of the calling
// thread until the end of the th thread

// If the waiting thread was
// forced to end, then the value of the parameter thread_return is a predefined
// value stored in the PTHREAD_CANCELED constant


// If a thread calls the function pthread_join and later on another thread
// calls the same function, the function will return an error code instead of
// blocking the execution of the thread

// Other attributes of a thread are:
// • the property of the thread to be joined by another thread calling the function pthread_join;
// • the dimension of the stack;
// • placing the stack in the address space of the process;
// • attributes related to the scheduling of the thread’s execution

// The way to set the attributes of a thread is to build a structure of type
// pthread_attr_t and modify the default values of these fields by using
// different functions

// The address of this structure must then be passed to the
// function pthread_create:
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);

// For now, the only important attribute is the one related to the possibility for
// calling the function pthread_join for a particular thread:
#include <pthread.h>
int pthread_attr_setdetachstate(
pthread_attr_t *attr,
int detachstate);
int pthread_attr_getdetachstate(
const pthread_attr_t* attr,
int *detachstate);

PTHREAD_CREATE_DETACHED: another thread can’t wait till the end of
the thread, so implicitly, the information about the termination status
of the thread is not kept.

PTHREAD_CREATE_JOINABLE: Can wait for the end of the thread. The
information about the termination status of the thread is kept until
obtaining them through pthread_join function. This is the default
value of the attribute of the thread

// Other functions for modifying and obtaining the attributes of a thread:
// pthread_attr_getstacksize and pthread_attr_setstacksize;
// pthread_attr_getstackaddr and pthread_attr_setstackaddr etc


// If a thread is set to be ”detached” then in the moment that thread ends, the
// system will no longer hold information about its termination status and all
// the allocated resources will be freed, thus calling function pthread_join
// won’t succeed for that thread:
int pthread_detach(pthread_t th);

// The PTHREADS standard says that a signal can only be sent to a process
// and not to a thread of that process

// in Linux, threads are implemented as
// special processes (so called lightweight processes)

// ***************fork() !!!!!!!!!!
// The PTHREADS standard says that even though the child process is
// identical with the parent process in content, the only active thread in the
// newly created process will be the one that called the fork function

// Taking into account the implementation of threads in Linux through processes, this behavior is obvious

#include <pthread.h>
pthread_atfork( void (*prepare)(void), // The function that will be executed in the parent process before a new process is created
void (*parent)(void), // The function that will be executed in the parent process before function fork finishes its execution
void (*child)(void)); // The function that will be executed in the child process before function fork finishes its execution

// Function pthread_atfork can be called more than one time, setting a list of functions that will be executed in the LIFO order