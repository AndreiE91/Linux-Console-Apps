#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char buf[100];
int **fd_matrix;
int no_processes;

void increment(int pid, long long MAX) {
    long long count = 0;
    //printf("Process %d started!\n", pid);
    for (count=0; count<=MAX; count++) {
        // Try to read to see if it is the current process's turn
        if (read(fd_matrix[pid][0], buf, 1) == -1) {
            perror("Error reading from pipe");
            exit(-1);
        }
        printf("Process %d at step %lld\n", pid + 1, count);
        //printf("Debug %d wants to write in: %d\n", pid, pid % no_processes + 1);
        // Tell the next process in the cycle that it is allowed to continue
        if (write(fd_matrix[(pid + 1) % no_processes][1], "*", 1) == -1) {
            perror("Error writing into pipe");
            exit(-1);
        }
    }
}


int main(int argc, char** argv) {
    if(argc != 3) {
        printf("Usage %s <number_of_processes> <count_max>\n", argv[0]);
        exit(-1);
    }
    no_processes = atoi(argv[1]);
    int count_max = atoi(argv[2]);
    
    fd_matrix = malloc(no_processes * sizeof(int*));
    for (int i = 0; i < no_processes; ++i) {
        fd_matrix[i] = malloc(2 * sizeof(int));
    }

    int* pid = malloc(no_processes * sizeof(int));

    for(int i = 0; i < no_processes; ++i) {
        pipe(fd_matrix[i]);
    }

    for(int i = 0; i < no_processes; ++i) {
        pid[i] = fork();

        if (pid[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid[i] == 0) {  // child process
            if(i == 0) {
                if(write(fd_matrix[0][1], "*", 1) == -1) {
                    perror("Error writing into pipe");
                    exit(-1);
                }
            }
            increment(i, count_max);
            exit(EXIT_SUCCESS);
        }
    }
    
    // Wait n times, once for every created child process
    for(int i = 0; i < no_processes; ++i) {
        wait(NULL);
    }
    free(pid);
    free(fd_matrix);
}
