#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_TH  100

#define SYNCHRONIZED

#ifdef SYNCHRONIZED
pthread_mutex_t lock;
pthread_cond_t cond;
#endif

int N, M;
long long count = 0;
int max_count;

int th_no = 0;
void* limited_area(void *arg)
{
    int th_id = *((int*) arg);

    long long aux;
    
    // ENTRANCE of the CRITICAL REGION
    // Here is where the thread must check (ask the OS) if it is safe to enter or must wait
    
    printf("Thread %d is trying to enter its critical region\n", th_id);

    for(int i = 0; i < M; ++i) {
        pthread_mutex_lock(&lock);
        while(count % N != th_id) {
            pthread_cond_wait(&cond, &lock);
        }

        aux = count;
        printf("Thread %d read count having value %lld\n", th_id, aux);
        ++aux;
        usleep(100);
        count = aux;
        printf("Thread %d incremented count to %lld\n", th_id, count);

        if(count == max_count) {
            pthread_cond_broadcast(&cond); // wake up all waiting threads to exit
        }
        else {
            pthread_cond_signal(&cond); // signal the next waiting thread to run
        }
            pthread_mutex_unlock(&lock);
    }

    // Check if exit from the limited area is allowed
    if (pthread_mutex_lock(&lock) != 0) {
    	perror("Cannot take the lock");
    	exit(4);
    }

    // signal that a new place is available
    if (pthread_cond_signal(&cond)  != 0) {
    	perror("Cannot signal the condition waiters");
    	exit(7);
    }

    if (pthread_mutex_unlock(&lock) != 0) {
    	perror("Cannot release the lock");
    	exit(5);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Bad number of parameters\n");
        printf("Usage: %s <no_of_threads> <max_no_if_threads_in_limited_area>\n", argv[0]);
        exit(1);
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    max_count = N * M;


#ifdef SYNCHRONIZED
    // Create the lock to provide mutual exclusion for the concurrent threads
    if (pthread_mutex_init(&lock, NULL) != 0) {
    	perror("Cannot initialize the lock");
    	exit(2);
    }
	
    if (pthread_cond_init(&cond, NULL) != 0) {
    	perror("Cannot initialize the condition variable");
    	exit(3);
    }
#endif

    int i;
    pthread_t th[MAX_TH];
    int th_args[MAX_TH];
    
    // Create the N threads
    for (i=0; i<N; i++) {
        th_args[i] = i;
        if (pthread_create(&th[i], NULL, limited_area, & th_args[i]) != 0 ) {
            perror("cannot create threads");
            exit(4);
        }
    }

    // Wait for the termination of the N threads created
    for (i=0; i<N; i++) {
        pthread_join(th[i], NULL);
    }


#ifdef SYNCHRONIZED
    // Remove the lock
    if (pthread_mutex_destroy(&lock) != 0) {
    	perror("Cannot destroy the lock");
    	exit(8);
    }
	
    if (pthread_cond_destroy(&cond) != 0) {
    	perror("Cannot destroy the condition variable");
    	exit(9);
    }
#endif

    return 0;
}
