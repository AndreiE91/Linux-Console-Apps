#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 100
#define NUM_NA 50
#define NUM_CL 50
int curr_na, curr_cl;
pthread_t threads[NUM_THREADS];
sem_t na_sem, cl_sem, na_mutex, cl_mutex;

#define NUM_THREADS2 120
#define NUM_H 80
#define NUM_O 40
int curr_h, curr_o;
pthread_t threads2[NUM_THREADS2];
sem_t h_sem, o_sem, h2_sem, h2o_mutex;
int count_h = 0;

#define NUM_THREADS3 140
#define NUM_H_ 40
#define NUM_O_ 80
#define NUM_S 20
int curr_h_, curr_o_, curr_s;
pthread_t threads3[NUM_THREADS3];
sem_t h__sem, o__sem, s_sem, h2_mutex, s_mutex, o_mutex;
int count__h = 0, count_o = 0;

//******NaCl
void *na_func(void *arg) {
    sem_wait(&na_mutex);
    printf("Na arrived!\n");
    sem_post(&na_sem);
    sem_wait(&cl_sem);
    printf("Na coupled!\n");
    sem_post(&na_mutex);
    pthread_exit(NULL);
}

void *cl_func(void *arg) {
    sem_wait(&cl_mutex);
    printf("Cl arrived!\n");
    sem_post(&cl_sem);
    sem_wait(&na_sem);
    printf("Cl coupled!\n");
    sem_post(&cl_mutex);
    pthread_exit(NULL);
}

//******H2O
void *h_func(void *arg) {
    sem_wait(&h_sem);

    printf("H\n");
    ++count_h;
    if(count_h % 2 == 0) {
        sem_post(&o_sem);
    } else {
        sem_post(&h_sem);
    }
    sem_wait(&h2o_mutex);
    pthread_exit(NULL);
}

void *o_func(void *arg) {
    sem_wait(&o_sem);
    printf("O\n");
    sem_post(&h_sem);
    sem_post(&h2o_mutex);
    sem_post(&h2o_mutex);
    pthread_exit(NULL);
}

//******H2SO4
void *h__func(void *arg) {
    sem_wait(&h__sem);
    printf("H\n");
    ++count__h;
    if(count__h % 2 == 0) {
        sem_post(&s_sem);
    } else {
        sem_post(&h__sem);
    }
    sem_wait(&h2_mutex);
    pthread_exit(NULL);
}

void *s_func(void *arg) {
    sem_wait(&s_sem);
    printf("S\n");
    sem_post(&o__sem);
    sem_wait(&s_mutex);
    pthread_exit(NULL);
}

void *o__func(void *arg) {
    sem_wait(&o__sem);
    printf("O\n");
    ++count_o;
    if(count_o % 4 == 0) {
        sem_post(&h2_mutex);
        sem_post(&h2_mutex);
        sem_post(&s_mutex);
        sem_post(&o_mutex);
        sem_post(&o_mutex);
        sem_post(&o_mutex);
        sem_post(&h__sem);
    } else {
        sem_post(&o__sem);
        sem_wait(&o_mutex);
    }
    pthread_exit(NULL);
}



int main() {
    srand(time(NULL));

//******NaCl
    printf("NaCl:\n");
    sem_init(&na_sem, 0, 0);
    sem_init(&cl_sem, 0, 0);
    sem_init(&na_mutex, 0, 1);
    sem_init(&cl_mutex, 0, 1);

    int i = 0;
    curr_cl = 0; curr_na = 0;
    while (curr_cl + curr_na < NUM_THREADS) {
        if(rand() % 2 == 0) {
            if(curr_cl < NUM_CL) {
                pthread_create(&threads[i], NULL, cl_func, NULL);
                ++curr_cl;
            }
        } else {
            if(curr_na < NUM_NA) {
                pthread_create(&threads[i], NULL, na_func, NULL);
                ++curr_na;
            }
        }
    }

    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&na_sem);
    sem_destroy(&cl_sem);
    sleep(1);

//******H2O
    printf("\n\nH2O:\n");
    sem_init(&h_sem, 0, 1);
    sem_init(&o_sem, 0, 0);
    sem_init(&h2o_mutex, 0, 2);

    i = 0;
    curr_h = 0; curr_o = 0;
    while (curr_h + curr_o < NUM_THREADS2) {
        if(rand() % 2 == 0) {
            if(curr_h < NUM_H) {
                pthread_create(&threads2[i], NULL, h_func, NULL);
                ++curr_h;
            }
        } else {
            if(curr_o < NUM_O) {
                pthread_create(&threads2[i], NULL, o_func, NULL);
                ++curr_o;
            }
        }
    }

    for (i = 0; i < NUM_THREADS2; ++i) {
        pthread_join(threads2[i], NULL);
    }

    sem_destroy(&h_sem);
    sem_destroy(&o_sem);

    sleep(1);

//******H2SO4
    printf("\n\nH2SO4:\n");
    sem_init(&h__sem, 0, 1);
    sem_init(&o__sem, 0, 0);
    sem_init(&s_sem, 0, 0);
    sem_init(&h2_mutex, 0, 2);
    sem_init(&s_mutex, 0, 1);
    sem_init(&o_mutex, 0, 4);
    sem_init(&h2_mutex, 0, 0);
    sem_init(&s_mutex, 0, 0);
    sem_init(&o_mutex,0, 0);

    i = 0;
    curr_h_ = 0; curr_o_ = 0, curr_s = 0;
    while (curr_h_ + curr_o_ + curr_s < NUM_THREADS3) {
        int randd = rand();
        if(randd % 3 == 0) {
            if(curr_h_ < NUM_H_) {
                pthread_create(&threads3[i], NULL, h__func, NULL);
                ++curr_h_;
            }
        } else if(randd % 3 == 1){
            if(curr_o_ < NUM_O_) {
                pthread_create(&threads3[i], NULL, o__func, NULL);
                ++curr_o_;
            }
        } else {
            if(curr_s < NUM_S) {
                pthread_create(&threads3[i], NULL, s_func, NULL);
                ++curr_s;
            }
        }
    }

    for (i = 0; i < NUM_THREADS3; ++i) {
        pthread_join(threads3[i], NULL);
    }

    //printf("H: %d  S: %d  O: %d", curr_h_, curr_s, curr_o_);


    sem_destroy(&h__sem);
    sem_destroy(&o__sem);   
    sem_destroy(&s_sem); 
    return 0;
}
