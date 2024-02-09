#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int get_line(int fd, char *line, int line_no, int max_length, int *line_length) {
    int current_line = 0;
    int i = 0;
    char c;

    lseek(fd, 0, SEEK_SET); //Moves cursor to beginning of file

    while (current_line < line_no && read(fd, &c, 1) > 0) { //Read each character until either file ends or desired line is found
        if (c == '\n') {
            current_line++;
        }
    }

    if (current_line < line_no) { //If desired line couldn't be found, terminate with an error(-1)
        return -1;
    }

    //lseek(fd, -1, SEEK_CUR);

    while (read(fd, &c, 1) > 0 && c != '\n') { //Add character by character into the output line, and if we add more than the maximum allowed terminate with an error
        if (i >= max_length) {
            return -1;
        }
        line[i++] = c;
    }

    //Important to add null terminator character and also assign the line length before terminating
    *line_length = i;
    if (i < max_length) {
        line[i] = '\0';
    } else {
        line[max_length - 1] = '\0'; //if i == max_length
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int fd, line_no, max_length, line_length, result;
    char line[100];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file> <line_number>\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);

    line_no = atoi(argv[2]);
    max_length = sizeof(line);

    result = get_line(fd, line, line_no, max_length, &line_length);
    if (result == 0) {
        printf("Line %d: %s (length=%d)\n", line_no, line, line_length);
    } else {
        printf("Line %d not found or too long\n", line_no);
    }

    close(fd);

    return 0;
}
