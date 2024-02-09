#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int n; // number of threads
int m; // number of times each thread increments the global variable

long long count = 0; // global variable to be incremented
pthread_mutex_t mutex; // mutex lock to protect the global variable
pthread_cond_t cond; // condition variable to signal threads to start over

void increment(void* arg)
{
    int th_id = *((int*) arg);

    for(int i = 0; i < m; ++i) {
        pthread_mutex_lock(&mutex);
        while(count % n != th_id) {
            pthread_cond_wait(&cond, &mutex);
        }
        ++count;
        printf("Thread %d incremented count to %lld\n", th_id, count);

        pthread_cond_broadcast(&cond); // signal the next waiting thread to run and all to exit when they reached the final count
    pthread_mutex_unlock(&mutex);
  }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
            printf("Bad number of parameters\n");
            printf("Usage: %s <no_of_threads> <no_of_times_each_thread_increments>\n", argv[0]);
            exit(1);
        }

    n = atoi(argv[1]);
    m = atoi(argv[2]);

  pthread_t threads[n];
  int th_ids[n];
  
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  for(int i = 0; i < n; ++i) {
    th_ids[i] = i;
    pthread_create(&threads[i], NULL, (void*) increment, &th_ids[i]);
  }
  
  for(int i = 0; i < n; ++i) {
    pthread_join(threads[i], NULL);
  }
  
  printf("Final count value is %lld\n", count);
  
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);

  return 0;
}
