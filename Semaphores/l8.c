#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

long long count = 0;
sem_t mutex;
int n_threads = 0;

void increment(int th_id)
{
  long long aux;
  aux = count;
  aux++;
  usleep(100);
  count = aux;
}

void *thread_func(void *th_id)
{
    sem_wait(&mutex); // Wait for the semaphore to be available
    int id = *(int*)th_id;
    increment(id);
    ++n_threads;
    sem_post(&mutex); // Release the semaphore
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <number of threads>\n", argv[0]);
        exit(1);
    }

    int n_threads = atoi(argv[1]);
    pthread_t threads[n_threads];
    int thread_ids[n_threads];

    sem_init(&mutex, 0, 1); // Initialize the semaphore with value 1

    for (int i = 0; i < n_threads; ++i) {
        thread_ids[i] = i;
        int res = pthread_create(&threads[i], NULL, thread_func, (void *)&thread_ids[i]);
        if (res != 0) {
            printf("Error creating thread %d.\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < n_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("count = %lld\n", count);
    printf("number of threads created = %d\n", n_threads);
    sem_destroy(&mutex); // Destroy the semaphore
    pthread_exit(NULL);
}
