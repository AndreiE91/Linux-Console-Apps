#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int buf[100];
int pipe1[2], pipe2[2];

void increment(int pid, long long MAX) {
    long long count = 0;
    for (count=0; count<=MAX; count++) {
        //printf("Debug %d\n", pid);
        pid == 1 ? read(pipe1[0], buf, 1) : read(pipe2[0], buf, 1);
        printf("Process %d at step %lld\n", pid, count);
        pid == 1 ? write(pipe2[1], "*", 1) : write(pipe1[1], "*", 1);
    }
}


int main() {
    pid_t pid, pid2;
    pipe(pipe1);
    pipe(pipe2);

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // child process
        // do something with pipe1[0] and pipe2[1]
        //printf("Write: %ld\n", write(pipe1[1], "*", 1));
        write(pipe1[1], "*", 1);
        increment(1, 10);
        exit(EXIT_SUCCESS);
    }

    pid2 = fork();

    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {  // second child process
        // do something with pipe1[0] and pipe2[1]
        increment(2, 10);
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
    wait(NULL);
}
