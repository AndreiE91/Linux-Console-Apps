#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
    int sv_newclient_pipe_fd;

    sv_newclient_pipe_fd = open("sv_newclient_pipe", O_WRONLY);
    if(sv_newclient_pipe_fd == -1) {
        perror("open pipe error");
        exit(EXIT_FAILURE);
    }

    close(sv_newclient_pipe_fd);

}