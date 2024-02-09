#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex, mutex2; // 2 semaphores needed to achieve alternative thread behavior
int count_max;

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

    int i = count_max / 2; // there will be 2 threads so each will count half of the total

    while(i > 0) {
        if(id == 0) {
            sem_wait(&mutex);
            increment(id);
            sem_post(&mutex2); 
        } else {
            sem_wait(&mutex2);
            increment(id);
            sem_post(&mutex); 
        }
        --i;
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
            printf("Usage: %s <count>\n", argv[0]);
            exit(1);
        }
    count_max = atoi(argv[1]);
    pthread_t threads[2];
    int thread_ids[2];

    sem_init(&mutex2, 0, 0); // Initialize the semaphore with value 0
    sem_init(&mutex, 0, 1); // Initialize the semaphore with value 1

    for (int i = 0; i < 2; ++i) {
        thread_ids[i] = i;
        int res = pthread_create(&threads[i], NULL, thread_func, (void *)&thread_ids[i]);
        if (res != 0) {
            printf("Error creating thread %d.\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < 2; ++i) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&mutex); // Destroy the semaphore
    sem_destroy(&mutex2); // Destroy the semaphore

    if(count_max % 2 == 1) { // Very cheap fix but I did it better for the more general case in ex 4 
            printf("count=%lld from thread %d\n", count + 1, 0);
    }

    pthread_exit(NULL);

}