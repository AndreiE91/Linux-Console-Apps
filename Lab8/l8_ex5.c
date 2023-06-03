#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

sem_t sem1;
sem_t sem2;

void street1_thread()
{
    while (1) {
        sem_wait(&sem1); // Wait for semaphore 1 to be available
        printf("Cars on street 1 are moving.\n");
        // Simulate cars moving on street 1
        sleep(1);
        sem_post(&sem1); // Release semaphore 1
    }
}

void street2_thread()
{
    while (1) {
        sem_wait(&sem2); // Wait for semaphore 2 to be available
        printf("Cars on street 2 are moving.\n");
        // Simulate cars moving on street 2
        sleep(1);
        sem_post(&sem2); // Release semaphore 2
    }
}

void traffic_light_thread()
{
    while (1) {
        sleep(rand() % 5 + 1); // Wait for a random amount of time
        sem_post(&sem1); // Allow cars on street 1 to move
        printf("Semaphore 1: GREEN, Semaphore 2: RED\n");
        sleep(5); // Wait for a fixed amount of time
        sem_wait(&sem1); // Stop cars on street 1
        sem_post(&sem2); // Allow cars on street 2 to move
        printf("Semaphore 1: RED, Semaphore 2: GREEN\n");
        sleep(5); // Wait for a fixed amount of time
        sem_wait(&sem2); // Stop cars on street 2
    }
}

void *thread_func(void *th_id)
{
    int id = *(int*)th_id;
    switch(id) {
        case 0: {
            traffic_light_thread();
            break;
        }
        case 1: {
            street1_thread();
            break;
        }
        case 2: {
            street2_thread();
            break;
        }
        default: {
            printf("Error\n");
            break;
        }
    }


    pthread_exit(NULL);

}

int main() {
    srand(time(NULL));
    // Create the semaphores
    sem_init(&sem1, 0, 0); // Initialize each semaphore with value 0
    sem_init(&sem2, 0, 0); 


    pthread_t threads[3];
    int thread_ids[3];

    for (int i = 0; i < 3; ++i) {
        thread_ids[i] = i;
        int res = pthread_create(&threads[i], NULL, thread_func, (void *)&thread_ids[i]);
        if (res != 0) {
            printf("Error creating thread %d.\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < 3; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Close semapphores
    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}


