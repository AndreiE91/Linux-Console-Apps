#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>


int no_persons;
int no_persons_inside;
int no_persons_who_left;

pthread_mutex_t mutex;
sem_t sem1, sem2, sem3;

// Comment out sleeps from person thread and uncomment the sleep from the thread creating for loop in main() to see that the light can turn on and off multiple times if room becomes empty in between

void person(void *arg) {
            
    int th_id = *((int*)arg);

    sleep(rand() % 4 + 2); // Person waits anywhere between 2 and 5 seconds before entering the room

    pthread_mutex_lock(&mutex);
    if(no_persons_inside == 0) {
        printf("First person with id %d announced its presence to the light controller\n", th_id);
        sem_post(&sem1);
    }
    sem_wait(&sem2); // Create a barrier
    sem_post(&sem2);
    printf("Person with id %d is entering the room\n", th_id);
    ++no_persons_inside;

    pthread_mutex_unlock(&mutex);


    sleep(rand() % 4 + 2); // Person waits anywhere between 2 and 5 seconds before leaving the room

    pthread_mutex_lock(&mutex);
    printf("Person with id %d is leaving the room\n", th_id);
    --no_persons_inside;
    ++no_persons_who_left;
    if(no_persons_inside == 0) {
        sem_post(&sem3);
    }
    pthread_mutex_unlock(&mutex);

}

void light_controller(void *arg) {
            
    int th_id = *((int*)arg);

    while(no_persons_who_left < no_persons) { // Only if there are still people who didn't enter it makes sense to try
        printf("Light controller with id %d is waiting for a person to want to enter\n", th_id);
        sem_wait(&sem1);
        printf("Light is ON\n");
        sem_post(&sem2); // Release the barrier

        sem_wait(&sem3);
        printf("Light is OFF\n");
        sem_wait(&sem2); // Reset barrier
    }

}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Error. Usage: %s <no_of_persons>\n", argv[0]);
        exit(1);
    }
    srand(time(NULL));

    no_persons = atoi(argv[1]);
    no_persons_inside = 0;
    no_persons_who_left = 0;

    // Initialize the semaphores
    sem_init(&sem1, 0, 0); 
    sem_init(&sem2, 0, 0); 
    sem_init(&sem3, 0, 0); 
    pthread_mutex_init(&mutex, NULL);

    pthread_t *threads = malloc((no_persons + 1) * sizeof(pthread_t));;
    int *th_ids = malloc((no_persons + 1) * sizeof(pthread_t));

    //Create threads for persons and one for light controller
    for(int i = 0; i <= no_persons; ++i) {
        th_ids[i] = i; 
        if(i == 0) {
            pthread_create(&threads[i], NULL, (void*) light_controller, &th_ids[i]);
        } else {
            pthread_create(&threads[i], NULL, (void*) person, &th_ids[i]);
        }
        //sleep(1);
    }

    // Wait for all threads to finish execution before terminating main thread
    for(int i = 0; i <= no_persons; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Destroy all
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);
    free(threads);
    free(th_ids);

}