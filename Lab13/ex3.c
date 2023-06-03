#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 

int pipe_c[2]; // Child pipe
int pipe_p[2]; // Parent pipe

int main(int argc, char** argv) {
    if (pipe(pipe_c) == -1) {
        perror("Cannot create pipe");
        exit(1);
    }
    if (pipe(pipe_p) == -1) {
        perror("Cannot create pipe");
        exit(1);
    }
    int pid = fork();
    if (pid == -1) {
        perror("Cannot create process");
        exit(1);
    }
    else if (pid == 0) {
        close(pipe_p[1]);
        close(pipe_c[0]);
        char buf[100];
        int size = sizeof("Sol-Do-La-Ti-Do-Re-Do");
        sprintf(buf, "%c%s", size, "Sol-Do-La-Ti-Do-Re-Do");
        write(pipe_c[1], buf, sizeof(buf));

        char size2;
        read(pipe_p[0], &size2, 1);
        read(pipe_p[0], buf, size2);
        printf("Child: %s\n", buf);
        wait(NULL);
    }
    else {
        close(pipe_c[1]);
        close(pipe_p[0]);
        char buf[100];
        int size = sizeof("Sol-Do-La-Fa-Me-Do-Re");
        sprintf(buf, "%c%s", size, "Sol-Do-La-Fa-Me-Do-Re");
        write(pipe_p[1], buf, sizeof(buf));

        char size2;
        read(pipe_c[0], &size2, 1);
        read(pipe_c[0], buf, size2);
        printf("Parent: %s\n", buf);
        wait(NULL);
    }
    return 0;
}