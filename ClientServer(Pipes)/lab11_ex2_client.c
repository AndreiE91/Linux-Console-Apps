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
    int result;

    // Open the named pipes
    fd = open(PIPE_NAME, O_RDWR);
    fd1 = open(PIPE_NAME1, O_RDONLY);

    while (1) {
        printf("Client: Enter two integers and an operator (+/-) separated by spaces or 'x' to exit: ");
        if(scanf("%d %c %d", &num1, &operator, &num2) < 3) {
            printf("Client: Exiting...\n");
            char operator = 'x';
            int num1 = 0;
            int num2 = 0;
            write(fd, &num1, sizeof(int));
            write(fd, &num2, sizeof(int));
            write(fd, &operator, sizeof(char));
            break;
        }

        // Write the data to the named pipe
        write(fd, &num1, sizeof(int));
        write(fd, &num2, sizeof(int));
        write(fd, &operator, sizeof(char));

        // Read the result from the named pipe
        read(fd1, &result, sizeof(int));

        printf("Client: Result: %d\n", result);
    }

    // Close the named pipes
    close(fd);
    close(fd1);

    return 0;
}
