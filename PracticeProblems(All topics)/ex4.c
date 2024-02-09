#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <dirent.h>

int pipe1[2]; // Child
int pipe2[2]; // Parent

// /home/osexam/LabSim/TEST1.txt
int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <file name> <dir name>\n", argv[0]);
        exit(1);
    }

    struct stat stats;
    lstat(argv[1], &stats);
    if (!S_ISREG(stats.st_mode)) {
        printf("Not a file\n");
        exit(1);
    }

    lstat(argv[2], &stats);
    if (!S_ISDIR(stats.st_mode)) {
        printf("Not a dir\n");
        exit(1);
    }

    if (pipe(pipe1) == -1) {
        perror("Cannot create pipe");
        exit(1);
    }
    if (pipe(pipe2) == -1) {
        perror("Cannot create pipe");
        exit(1);
    }
    
    int pid1, pid2;

    pid1 = fork();

    if (pid1 == -1) {
        perror("Cannot create process");
        exit(1);
    }
    else if (pid1 == 0) { // Inside P2
        pid2 = fork();
        if (pid2 == -1) {
            perror("Cannot create process");
            exit(1);
        }
        else if (pid2 == 0) { // Inside P3

        }
        else {
            wait(NULL);
        }
    }
    else { // Inside P1
        int fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            perror("Cannot open file");
            exit(-1);
        }
        char curr_char = 'n';
        int bytes_till_line = 0;
        int last_pos = 0;
        int size = lseek(fd, 0, SEEK_END);
        lseek(fd, SEEK_SET, 0);
        int line_index = 0;
        while(last_pos < size) {
            while(curr_char != '\n') {
                read(fd, &curr_char, 1);
                ++bytes_till_line;
                printf("%c ", curr_char);
            }
            lseek(fd, SEEK_SET, last_pos);
            char* line = malloc(bytes_till_line * sizeof(char) + 1);
            printf("Bytes till line: %d \n", bytes_till_line);
            if(line == NULL) {
                perror("Cannot alloate memory");
                exit(-1);
            }
            read(fd, &line, bytes_till_line);
            line[bytes_till_line - 1] = '\0';
            last_pos += bytes_till_line;
            bytes_till_line = 0;
            printf("Line %d:%s", line_index, line);
            ++line_index;
        }

        wait(NULL);
        close(fd);
    }

    return 0;
}