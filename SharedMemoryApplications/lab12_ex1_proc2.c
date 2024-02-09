#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FIFO_NAME "./myfifo11"
#define FIFO_NAME1 "./myfifo12"

int main(int argc, char** argv) {
    if (argc != 3)
    {
        printf("Usage: %s <name of shared memory object> <size of shared memory>\n", argv[0]);
        exit(-1);
    }

    int shm_fd;
    char *shared_char = NULL;

    int fd, fd1;
    char message[4096];

    int size = atoi(argv[2]);

    // Open the named pipe for reading
    printf("proc2: reached fd opens");
    fd = open(FIFO_NAME, O_RDWR);
    fd1 = open(FIFO_NAME1, O_RDWR);
    if(fd == -1) {
            perror("open pipe error");
            exit(EXIT_FAILURE);
    }
    if(fd1 == -1) {
            perror("open pipe error");
            exit(EXIT_FAILURE);
    }

    printf("proc2: Reached shm open\n");
    shm_fd = shm_open(argv[1], O_CREAT | O_RDWR, 0664);
    if(shm_fd < 0) {
        perror("Could not aquire shm");
        return 1;
    }

    shared_char = (char*)mmap(0, size * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    if(shared_char == MAP_FAILED) {
        perror("Could not map the shared memory");
        close(shm_fd);
        exit(2);
    }
    
    close(shm_fd);

    // Read from a to z until size is reached
    int count = 0;
    printf("proc2: Reached loop\n");
    while(count < size) {
        // Consume 1 permission each time, essentially waiting for process 1 to give permission
        //printf("proc2: Reached read\n");
        if(read(fd, message, 1) == -1) {
            perror("read pipe error");
            exit(1);
        }
        if(count % 20 == 0) {
            printf("\n");
        }
        printf("%c ", shared_char[count]);
        ++count;
        // Give 1 permission to semaphore 2, allowing process 1 an iteration
        //printf("proc2: Reached write\n");
        if(write(fd1, "*", 1) == -1) {
            perror("write pipe error");
            exit(1);
        }
    }

    munmap(shared_char, size * sizeof(char));
    shared_char = NULL;

    shm_unlink(argv[1]);

    // Close the named pipes
    close(fd);
    close(fd);
    printf("\n");

    return 0;
}