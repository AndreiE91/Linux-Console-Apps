#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define EMPTY -1
#define MISSIONARY 0
#define CANNIBAL 1
#define NO_SEATS 3

pthread_mutex_t mutex, mutex2, boat_mutex, mutex_instantiated;
pthread_cond_t cond, cond2, cond_instantiated;

int n, no_missionaries, no_cannibals; // How many were created of each
int count; // How many threads arrived

int curr_missionaries, curr_cannibals;

bool all_instantiated;

// Since we have only 3 seats and boat won't leave without being fully loaded, we must have n % 3 = 0 as a constraint.
// Strategy is as follows: take care of missionaries first, as cannibal can go whichever way

// Try to pair 2 missionaries with a cannibal.
// When there are no more missionaries/cannibals left, just pair what's left together until nothing remains

void missionary(void *arg) {
    int th_id = *((int*)arg);
    printf("Missionary with id %d created\n", th_id);

    pthread_mutex_lock(&mutex_instantiated);
    ++count;

    while(!all_instantiated) {
        pthread_cond_wait(&cond_instantiated, &mutex_instantiated);
    }
    pthread_mutex_unlock(&mutex_instantiated);

    pthread_mutex_lock(&mutex);
    while((curr_missionaries == 2 && curr_cannibals == 1) || (curr_missionaries == 3 && no_cannibals == 0)) {
        printf("Thread %d is waiting...\n", th_id);
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Thread %d is incrementing its value\n", th_id);
    ++curr_missionaries;
    printf("Missionary %d in boat\n", th_id);
   
   pthread_mutex_unlock(&mutex);
}

void cannibal(void *arg) {
    int th_id = *((int*)arg);
    printf("Cannibal with id %d created\n", th_id);

    pthread_mutex_lock(&mutex_instantiated);
    ++count;
    while(!all_instantiated) {
        pthread_cond_wait(&cond_instantiated, &mutex_instantiated);
    }
    pthread_mutex_unlock(&mutex_instantiated);

    pthread_mutex_lock(&mutex);
    while((curr_cannibals == 3 && curr_missionaries == 0) || (curr_cannibals >= 1 && curr_missionaries == 2)) {
        pthread_cond_wait(&cond2, &mutex);
    }
    
    ++curr_cannibals;
    printf("Cannibal %d in boat\n", th_id);

    pthread_mutex_unlock(&mutex);
    
}

void boatt(void *arg) {
    int th_id = *((int*)arg);
    printf("Boat with id %d created\n", th_id);

    while(count < n) {
    }

    printf("\nTotal missionaries created: %d | Total cannibals created: %d\n", no_missionaries, no_cannibals);
    printf("\n\nBEGIN:\n");
    all_instantiated = true;
    pthread_cond_broadcast(&cond_instantiated);

    while(no_cannibals + no_missionaries > 0) {
        if(curr_missionaries + curr_cannibals >= 3) {
            pthread_mutex_lock(&mutex);
            printf("Boat with id %d is leaving with {", th_id);
            for(int i = 0; i < NO_SEATS; ++i) {
                if(curr_cannibals > 0) {
                    printf("CANNIBAL");
                    --curr_cannibals;
                    --no_cannibals;
                } else if(curr_missionaries > 0) {
                    printf("MISSIONARY");
                    --curr_missionaries;
                    --no_missionaries;
                } 
                i == NO_SEATS - 1 ? printf("}\n") : printf(",");
            }
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&cond2);
            pthread_cond_broadcast(&cond);
            sleep(2);
        }
    }
}


int main(int argc, char** argv) {
    srand(time(0));
    printf("Input an n divisible by %d:\n", NO_SEATS);
    if(argc != 2) {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        exit(-1);
    }
    n = atoi(argv[1]);
    if(n % 3) {
        printf("n is not divisible by %d\n", NO_SEATS);
        exit(-2);
    }
    printf("\n\nSETUP:\n");

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_instantiated, NULL);
    pthread_mutex_init(&boat_mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&cond2, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond_instantiated, NULL);

    pthread_t threads[n + 1];
    int th_ids[n + 1];
    
    // Initialize global variables
    curr_cannibals = 0;
    curr_missionaries = 0;
    all_instantiated = false;
    count = 0;

    // Create threads representing either cannibals or missionaries
    for(int i = 0; i < n; ++i) {
        th_ids[i] = i;
        if(rand() % 2) {
            pthread_create(&threads[i], NULL, (void*) missionary, &th_ids[i]);
            ++no_missionaries;
        } else {
            pthread_create(&threads[i], NULL, (void*) cannibal, &th_ids[i]);
            ++no_cannibals;
        }
    }

    // Create the thread representing the boat
    th_ids[n] = n;
    pthread_create(&threads[n], NULL, (void*) boatt, &th_ids[n]);

    // Wait for all threads to finish execution
    for(int i = 0; i <= n; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_instantiated);
    pthread_mutex_destroy(&boat_mutex);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&cond_instantiated);
    pthread_mutex_destroy(&mutex2);
    pthread_cond_destroy(&cond2);

}