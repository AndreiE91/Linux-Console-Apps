#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CARS 50
#define GENERATE_AMOUNT 1000

int no_threads;
int no_cars = 0;
pthread_mutex_t mutex;
sem_t semaphore;

void* car_parking(void* arg) {
    sem_wait(&semaphore);

    pthread_mutex_lock(&mutex);
    ++no_cars;
    pthread_mutex_unlock(&mutex);

    printf("No of cars in the park: %d\n", no_cars);
    usleep(2000);

    pthread_mutex_lock(&mutex);
    --no_cars;
    pthread_mutex_unlock(&mutex);

    sem_post(&semaphore);

    pthread_exit(NULL);
}

int main(int argc, char** argv) {

    if(argc != 1) {
        printf("Usage: %s\n", argv[0]);
        exit(1);
    }

    no_threads = GENERATE_AMOUNT;
    int th_ids[no_threads];
    pthread_t threads[no_threads];

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    // Initialize semaphore
    if (sem_init(&semaphore, 0, MAX_CARS) != 0) {
        perror("Failed to initialize semaphore");
        exit(EXIT_FAILURE);
    }

    // Create N threads
    for (int i = 0; i < no_threads; ++i) {
        th_ids[i] = i;

        int threadCreateResult = pthread_create(&threads[i], NULL, car_parking, (void*)&th_ids[i]);
        if (threadCreateResult != 0) {
            printf("Failed to create thread %d\n", i);
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < no_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    // Destroy semaphore
    if (sem_destroy(&semaphore) != 0) {
        perror("Failed to destroy semaphore");
        exit(EXIT_FAILURE);
    }

    return 0;
}