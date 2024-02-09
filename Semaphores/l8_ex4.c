#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t *mutex; // N threads, will allocate in main()
int count_max, num_threads, remainderr; // remainder is a built-in function, so I need to use another name

sem_t mutex_rem; // semaphore protecting global variable "remainderr"


long long count = 0;
void increment(int th_id)
{
    long long aux;
    aux = count;
    ++aux;
    printf("count=%lld from thread %d\n", count, th_id);
    count = aux;
}

void *thread_func(void *th_id)
{
    int id = *(int*)th_id;

    int i = count_max / num_threads; // there will be N threads so each will count for N-th of the total
    while(i > 0) {
        sem_wait(&mutex[id]); // Wait for thread's semaphore to become available
        increment(id);
        sem_post(&mutex[(id + 1) % num_threads]); // Release the next thread's semaphore
        --i;
    }
    // Account for remainder
    while(remainderr > 0) {
        sem_wait(&mutex[id]); // Wait for thread's semaphore to become available
        
        if(remainderr <= 0) { // Since there are multiple threads, they will already be inside the while loop even after remainderr becomes 0 or less
            sem_post(&mutex[(id + 1) % num_threads]); // Release the next thread's semaphore
            break;
        }
        increment(id);
        sem_post(&mutex[(id + 1) % num_threads]); // Release the next thread's semaphore

        sem_wait(&mutex_rem); // Wait for mutex_rem to become available
        --remainderr;
        sem_post(&mutex_rem); // Release mutex_rem after modifying the global variable
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
            printf("Usage: %s <count> <num_threads>\n", argv[0]);
            exit(1);
        }
    count_max = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    remainderr = count_max % num_threads; // Find the remainder so we don't miss any count in case of non perfectly divisible count_max and num_threads
    if(num_threads <= 0 || num_threads > 1000) { // Add failsafe to prevent memory overload or negative array size errors
        printf("Invalid number of threads. It must be between 1 and 1000, including those values\n");
        exit(1);
    }
    pthread_t *threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t)); // Dynamically allocate memory for threads
    int *thread_ids = (int*) malloc(num_threads * sizeof(int)); // Dynamically allocate memory for thread_ids

    mutex = (sem_t*) malloc(num_threads * sizeof(sem_t)); // Dynamically allocate memory for mutex

    sem_init(&mutex_rem, 0, 1); // Initialize mutex_rem with value 1

    for (int i = 0; i < num_threads; ++i) {
        sem_init(&mutex[i], 0, 0); // Initialize each semaphore with value 0
    }

    sem_post(&mutex[0]); // Start with the first semaphore available


    for (int i = 0; i < num_threads; ++i) {
        thread_ids[i] = i;
        int res = pthread_create(&threads[i], NULL, thread_func, (void *)&thread_ids[i]);
        if (res != 0) {
            printf("Error creating thread %d.\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        sem_destroy(&mutex[i]); // destroy each semaphore
    }

    sem_destroy(&mutex_rem); // destroy mutex_rem semaphore

    free(threads);
    free(thread_ids);
    free(mutex);

    pthread_exit(NULL);

}