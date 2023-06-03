#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define MAX_CLIENTS 10

pthread_mutex_t lock;

pthread_t client_threads[MAX_CLIENTS];
int num_clients = 0;

void* handleClient(void* arg) {
    int id = *((int*)arg);

    printf("Server: Client %d connected\n", id);



    pthread_exit(NULL);
}

int main() {
    int sv_newclient_pipe_fd;

    if(unlink("sv_newclient_pipe") == -1) {
        printf("No pipe found to unlink, all good!\n");
    } else {
        printf("Previous pipe unlinked to remove data interference, all good now!\n");
    }

    if(mkfifo("sv_newclient_pipe", 0666) == -1) {
        perror("mkfifo error");
        exit(EXIT_FAILURE);
    }

    if(pthread_mutex_init(&lock, NULL) != 0) {
        perror("");
    }

    printf("Server: Server is running, waiting for clients...\n");

    while(1){
        sv_newclient_pipe_fd = open("sv_newclient_pipe", O_RDONLY);
        if(sv_newclient_pipe_fd == -1) {
            perror("open pipe error");
            exit(EXIT_FAILURE);
        }
        printf("New reader arrived!\n");
        close(sv_newclient_pipe_fd);
    }
}