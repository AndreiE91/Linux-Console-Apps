#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define STREET_A 0
#define STREET_B 1
#define SEM_DELAY 8 // Number of seconds per semaphore switch

int n, no_cars_entered; // number of threads
pthread_mutex_t mutex, finish_cars_mutex;
pthread_cond_t cond, street_a, street_b, finish_cars;
bool exists_traffic_light, street_a_green, street_b_green, all_cars_instantiated, still_cars_a, still_cars_b; // Tried to do smth with these last two booleans but things seem to work way better without them

void car(void* arg) {
    int th_id = *((int*) arg);

    int street = rand() % 2 == 0 ? STREET_A : STREET_B; // For each car, the street it is placed on will be randomly chosen

    pthread_mutex_lock(&mutex);
    while(!exists_traffic_light) {
        pthread_cond_wait(&cond, &mutex);
    }

    switch(street) {
        case STREET_A: {
            printf("Car %d has entered street A\n", th_id);
            break;
        }
        case STREET_B: {
            printf("Car %d has entered street B\n", th_id);
            break;
        }
        default: {
            printf("Invalid street number for car %d\n", th_id);
            break;
        }
    }
    ++no_cars_entered;
    pthread_mutex_unlock(&mutex);

    // Block all cars until traffic light says otherwise
    pthread_mutex_lock(&finish_cars_mutex);
    printf("Car %d has arrived at the intersection\n", th_id);
    while(no_cars_entered < n) {
        pthread_cond_wait(&finish_cars, &finish_cars_mutex);
    }
    all_cars_instantiated = true;
    pthread_mutex_unlock(&finish_cars_mutex);

    while(true) {
        pthread_mutex_lock(&mutex);

        switch(street) {
            case STREET_A: {
                while(!street_a_green) {
                    pthread_cond_wait(&street_a, &mutex);
                }
                break;
            }
            case STREET_B: {
                while(!street_b_green) {
                    pthread_cond_wait(&street_b, &mutex);
                }
                break;
            }
            default: {
                printf("Invalid street number for car %d\n", th_id);
                break;
            }
        }
        
        switch(street) {
            case STREET_A: {
                still_cars_a = true;
                break;
            }
            case STREET_B: {
                still_cars_b = true;
                break;
            }
            default: {
                printf("Invalid street number for car %d\n", th_id);
                break;
            }
        }
        printf("Car %d is driving through the intersection(from street ", th_id);
        street == STREET_A ? printf("A)\n") : printf("B)\n");
        sleep(1);

        printf("Car %d has left the intersection\n", th_id);
        switch(street) {
            case STREET_A: {
                still_cars_a = false;;
                break;
            }
            case STREET_B: {
                still_cars_b = false;
                break;
            }
            default: {
                printf("Invalid street number for car %d\n", th_id);
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }

}

void traffic_light(void* arg) {
    int th_id = *((int*) arg);

    printf("Traffic light with thread id %d is up and running\n", th_id);
    exists_traffic_light = true;
    pthread_cond_broadcast(&cond);
    
    while(!all_cars_instantiated) {
    }
    pthread_cond_broadcast(&finish_cars);

    while(true) {

        printf("Street A light: RED | Street B light: RED\n");
        street_a_green = false;
        street_b_green = false;
        sleep(SEM_DELAY);

        printf("Street A light: GREEN | Street B light: RED\n");
        street_a_green = true;
        street_b_green = false;
        pthread_cond_broadcast(&street_a);
        sleep(SEM_DELAY);

        printf("Street A light: RED | Street B light: GREEN\n");
        street_a_green = false;
        street_b_green = true;
        pthread_cond_broadcast(&street_b);
        sleep(SEM_DELAY);
    }

}

int main(int argc, char** argv) {
    srand(time(0));
    if(argc != 2) {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        exit(-1);
    }
    n = atoi(argv[1]);

    pthread_t threads[n + 1];
    int th_ids[n + 1];

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&finish_cars_mutex, NULL);
    pthread_cond_init(&finish_cars, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&street_a, NULL);
    pthread_cond_init(&street_b, NULL);

    
    exists_traffic_light = false;
    all_cars_instantiated = false;
    still_cars_a = true;
    still_cars_b = true;
    no_cars_entered = 0;

    // Create threads representing the cars
    for(int i = 0; i < n; ++i) {
        th_ids[i] = i;
        pthread_create(&threads[i], NULL, (void*) car, &th_ids[i]);
    }

    //Create the final thread which will represent the traffic light
    th_ids[n] = n;
    pthread_create(&threads[n], NULL, (void*) traffic_light, &th_ids[n]);

    // IRRELEVANT IN THIS CASE AS THERE ARE INFINITE WHILE LOOPS. STILL GOOD PRACTICE TO HAVE THIS PORTION OF CODE BUT ONLY IF ADDITIONAL LOGIC IS IMPLEMENTED TO ALLOW USER TO QUIT USING
    // OTHER METHOD THAN CRTL+C FROM THE TERMINAL

    // Wait for all threads to finish execution
    for(int i = 0; i < n + 1; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&finish_cars_mutex);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&street_a);
    pthread_cond_destroy(&street_b);
    pthread_cond_destroy(&finish_cars);

}
