#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Usage: %s <ls argument>\n", argv[0]);
        exit(-1);
    }

    int pid = fork();

    if(pid == 0) {
        printf("Child process:\n");
        printf("Own PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());
        //system("ls /home");
        execlp("ls", "ls", "-l", argv[1], NULL);
        return 0;
    } else {
        printf("Parent process:\n");
        printf("Child PID: %d\n", pid);
        printf("Own PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());
        printf("Exit code of child: %d\n", wait(0));
        return 0;
    }
}