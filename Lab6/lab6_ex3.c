#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
    {
        // error case
        perror("Cannot create a new child");
        exit(1);
    }
    case 0:
    {
        printf("P2 pid = %d, ppid = %d\n", getpid(), getppid());
        // P2 reaches here
        break;
    }
    default:
    {
        // parent
        printf("P1 pid = %d, ppid = %d\n", getpid(), getppid());
        pid_t pid1 = fork(); // parent creates P3
        switch (pid1)
        {
        case -1:
        {
            // error case
            perror("Cannot create a new child");
            exit(1);
        }
        case 0:
        {
            // P3 reaches here, will create P5
            printf("P3 pid = %d, ppid = %d\n", getpid(), getppid());
            pid_t pid2 = fork();
            switch (pid2)
            {
            case -1:
            {
                // error case
                perror("Cannot create a new child");
                exit(1);
            }
            case 0:
            {
                // P5 reaches here
                printf("P5 pid = %d, ppid = %d\n", getpid(), getppid());
                break;
            }
            default:
            {
                printf("P3 pid = %d, ppid = %d\n", getpid(), getppid());
                wait(0); //P3 waits for P5 to terminate
                break;
            }
            }
            break;
        default:
        {
            // Instructions to be executed by P1
            pid_t pid3 = fork(); // P1 creates P4
            switch (pid3)
            {
            case -1:
            {
                // error case
                perror("Cannot create a new child");
                exit(1);
            }
            case 0:
            {
                // P4 reaches here
                printf("P4 pid = %d, ppid = %d\n", getpid(), getppid());
                break;
            }
            default:
            {
                // P1 reaches here
                printf("P1 pid = %d, ppid = %d\n", getpid(), getppid());
                wait(0);
                wait(0);
                wait(0); //Wait for all three child processes to terminate
                break;
            }
            }
            break;
            }
        }
        }
        }
    }
    }
}