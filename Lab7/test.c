// Use gcc -Wall -Werror progr.c –lpthread –o progr to compile
#include <stdio.h>      // for printf()
#include <pthread.h>    // for pthread_t, pthread_create(), pthread_join(), pthread_sigmask()
#include <signal.h>     // for signal(), SIGUSR1, sigset_t, sigemptyset(), sigaddset()
#include <sys/types.h>
