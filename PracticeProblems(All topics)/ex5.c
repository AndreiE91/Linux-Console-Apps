#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int totalSum = 0;
int no_threads;
pthread_mutex_t mutex;
int *sumChunks;

// Thread function to calculate partial sum
void* calculateSum(void* arg) {
    int id = *((int*)arg);

    int partialSum = 0;
    int start = sumChunks[id] - no_threads + 1;
    int end = sumChunks[id];
    for (int i = start; i <= end; ++i) {
        partialSum += i;
    }

    printf("Thread %d: partialSum = %d\n", id, partialSum);

    pthread_mutex_lock(&mutex);
    totalSum += partialSum;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <no_threads>\n", argv[0]);
        return 1;
    }

    no_threads = atoi(argv[1]);
    int th_ids[no_threads];
    pthread_t threads[no_threads];
    sumChunks = malloc(no_threads * sizeof(int));
    for(int i = 0; i < no_threads; ++i) {
        sumChunks[i] = (i + 1) * no_threads;
    }

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    // Create N threads
    for (int i = 0; i < no_threads; ++i) {
        th_ids[i] = i;

        int threadCreateResult = pthread_create(&threads[i], NULL, calculateSum, (void*)&th_ids[i]);
        if (threadCreateResult != 0) {
            printf("Failed to create thread %d\n", i);
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < no_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Final sum = %d\n", totalSum);

    free(sumChunks);
    pthread_mutex_destroy(&mutex);

    return 0;
}
