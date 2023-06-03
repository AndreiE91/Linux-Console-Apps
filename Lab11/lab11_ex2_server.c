#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define PIPE_NAME "my_pipe"
#define PIPE_NAME1 "my_pipe1"


int main() {
    int fd, fd1;
    char operator;
    int num1, num2;
    int result = 0;

    // Create the named pipes
    mkfifo(PIPE_NAME, 0666);
    mkfifo(PIPE_NAME1, 0666);

    printf("Server: Waiting for clients...\n");

    // Open the named pipes
    fd = open(PIPE_NAME, O_RDWR);
    fd1 = open(PIPE_NAME1, O_WRONLY);

    while (1) {
        // Read the data from the named pipe
        read(fd, &num1, sizeof(int));
        read(fd, &num2, sizeof(int));
        read(fd, &operator, sizeof(char));
        printf("%d %c %d\n", num1, operator, num2);

        if (operator == 'x') {
            printf("Server: Exiting...\n");
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
        write(fd1, &result, sizeof(int));

    }
    // Close the named pipes
    close(fd);
    close(fd1);

    // Remove the named pipes
    unlink(PIPE_NAME);
    unlink(PIPE_NAME1);

    return 0;
}
