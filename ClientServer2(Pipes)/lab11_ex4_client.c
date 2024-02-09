#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define PIPE_NAME2 "server_pipe" // Pipe used for receiving id from server
#define PIPE_NAME3 "server_pipe1" // Pipe used for announcing server of a client's presence
char PIPE_NAME[25]; 
char PIPE_NAME1[25];

int main() {
    int fd, fd1, fd2, fd3;
    char operator;
    int num1, num2;
    int result;
    int id;

    // Open the named pipes
    fd2 = open(PIPE_NAME2, O_RDONLY);
    fd3 = open(PIPE_NAME3, O_WRONLY);

    // For each client, before entering its action phase, announce the server to reserve a thread for it
    // Request a client id from server
    write(fd3, "*", sizeof(char));

    // Stall until it is given
    read(fd2, &id, sizeof(int));

    // Generate set of pipes for client i's needs
    strcpy(PIPE_NAME, "data_pipe");
    strcpy(PIPE_NAME1, "result_pipe");

    // Append the id integer to the strings
    char id_str1[25];
    char id_str2[25];

    sprintf(id_str1, "%s%d", PIPE_NAME2, id);
    strcpy(PIPE_NAME2, id_str1);
    sprintf(id_str2, "%s%d", PIPE_NAME3, id + 1);
    strcpy(PIPE_NAME3, id_str2);

    fd = open(PIPE_NAME, O_WRONLY);
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
    close(fd2);

    return 0;
}
