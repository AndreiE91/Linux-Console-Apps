#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex, mutex2;


int th_no = 0;
void limited_area(void)
{  
  sem_wait(&mutex2); // Wait for the semaphore to be available
  ++th_no; // aici trebuie implementat semafor cu 1 pt protejarea variabilei globale th_no
  sem_post(&mutex2); // Release the semaphore

  usleep(100);
  printf("The no of threads in the limited area is: %d\n", th_no);

  sem_wait(&mutex2); // Wait for the semaphore to be available
  --th_no; // si aici la fel
  sem_post(&mutex2); // Release the semaphore
}

void *thread_func(void *th_id)
{
    sem_wait(&mutex); // Wait for the semaphore to be available
    limited_area();
    sem_post(&mutex); // Release the semaphore
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
            printf("Usage: %s <number of threads> <number of maximum threads at once>\n", argv[0]);
            exit(1);
        }
    int n_threads = atoi(argv[1]);
    int m = atoi(argv[2]);
    pthread_t threads[n_threads];
    int thread_ids[n_threads];

    sem_init(&mutex2, 0, 1); // Initialize the semaphore with value 1
    sem_init(&mutex, 0, m); // Initialize the semaphore with value m given in the command line

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

    printf("Final value of th_no = %d\n", th_no);
    printf("number of threads created = %d\n", n_threads);
    sem_destroy(&mutex); // Destroy the semaphore
    sem_destroy(&mutex2); // Destroy the semaphore
    pthread_exit(NULL);

}