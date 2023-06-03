#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define PIPE_NAME2 "server_pipe" // Pipe used for sending id to client
#define PIPE_NAME3 "server_pipe1" // Pipe used for being announced by client that is has arrived

#define MAX_CLIENTS 100
char PIPE_NAME[25]; // Templates for client data read and result send write pipes respectively
char PIPE_NAME1[25];
pthread_mutex_t mutex; // mutex lock to protect the global variable
char PIPE_NAME4[25]; 
char PIPE_NAME5[25];

int no_clients;

void* handleClient(void* arg) {
    ++no_clients;
    pthread_mutex_unlock(&mutex);

    int id = *((int*)arg);
    int num1, num2, result;
    char operator;

    int fd2, fd3;

    // Generate set of pipes for client i's needs
    strcpy(PIPE_NAME4, "data_pipe");
    strcpy(PIPE_NAME5, "result_pipe");

    // Append the id integer to the strings
    char id_str1[50];
    char id_str2[50];

    sprintf(id_str1, "%s%d", PIPE_NAME4, id);
    strcpy(PIPE_NAME4, id_str1);
    sprintf(id_str2, "%s%d", PIPE_NAME5, id + 1);
    strcpy(PIPE_NAME5, id_str2);

    mkfifo(PIPE_NAME4, 0666);
    mkfifo(PIPE_NAME5, 0666);

    fd2 = open(PIPE_NAME4, O_RDONLY);
    fd3 = open(PIPE_NAME5, O_WRONLY);

    while (1) {
        // Read the data from the named pipe
        read(fd2, &num1, sizeof(int));
        read(fd2, &num2, sizeof(int));
        read(fd2, &operator, sizeof(char));
        printf("Client %d: %d %c %d\n", (id / 2), num1, operator, num2);

        if (operator == 'x') {
            printf("Client %d (inside server): Exiting...\n", (id / 2));
            break;
        }

        if (operator == '+') {
            result = num1 + num2;
        } else if (operator == '-') {
            result = num1 - num2;
        } else {
            printf("Server: Invalid operator\n");
            continue;
        }

        // Write the result to the named pipe
        write(fd3, &result, sizeof(int));
    }

    close(fd2);
    close(fd3);
    pthread_exit(NULL);
}

int main() {
    int fd, fd1;

    // Create the named pipes
    mkfifo(PIPE_NAME2, 0666); // Pipe used for sending id to client
    mkfifo(PIPE_NAME3, 0666); // Pipe used for being announced by client that is has arrived

    // Open the named pipes
    fd = open(PIPE_NAME2, O_WRONLY);
    fd1 = open(PIPE_NAME3, O_RDONLY);

    char buf[100];
    no_clients = 0;
    pthread_t threads[MAX_CLIENTS];

    printf("Server: Waiting for clients...\n");

    while(1) {
        // Wait to be announced that a client has arrived and is waiting to be served
        pthread_mutex_lock(&mutex);
        read(fd1, &buf, sizeof(char));
        int arg = (no_clients + 1) * 2;
        pthread_create(&threads[no_clients], NULL, handleClient, (void *)&arg); // First client will operate on pipes 2 and 1, second on 4 and 3...
        ++no_clients; // Therefore first client id will be 2, second will be 4 and so on to subtract 1 from it each time and be sure it is a new free id for naming the pipes
    }

    // Since server is running in an infinte while loop waiting for clients, code below will never be executed.
    // However, this does not hinder the code's efficiency and it is rather a convention
    // Besides Ctrl+C, there could be implemented another more sophisticated exit mechanism, which is why I leave it here in case of future developments

    // Wait for the client threads to complete
    for(int i = 0; i < no_clients; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Close the pipes
    close(fd);
    close(fd1);

    // Remove the named pipes
    unlink(PIPE_NAME2);
    unlink(PIPE_NAME3);

    return 0;
}
