#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

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

    //Unlink previous pipes to ensure data constistency
    if(unlink(FIFO_NAME) == -1) {
        printf("proc1: No pipe found to unlink, all good!\n");
    } else {
        printf("proc1: Previous pipe %s unlinked to remove data interference, all good now!\n", FIFO_NAME);
    }
    if(unlink(FIFO_NAME1) == -1) {
        printf("proc1: No pipe found to unlink, all good!\n");
    } else {
        printf("proc1: Previous pipe %s unlinked to remove data interference, all good now!\n", FIFO_NAME1);
    }
    printf("proc1: Reached mkfifo\n");
    //Create the named pipes (FIFO)
    if(mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo error");
        exit(EXIT_FAILURE);
    }
    if(mkfifo(FIFO_NAME1, 0666) == -1) {
        perror("mkfifo error");
        exit(EXIT_FAILURE);
    }

    // Open the named pipes for writing
    printf("proc1: Reached open\n");
    
    fd = open(FIFO_NAME, O_RDWR);
    if(fd == -1) {
            perror("open pipe error");
            exit(EXIT_FAILURE);
    }
    fd1 = open(FIFO_NAME1, O_RDWR);
    if(fd1 == -1) {
            perror("open pipe error");
            exit(EXIT_FAILURE);
    }

    // Initialize 1 permission to semaphore 2
    printf("proc1: Reached init write\n");
    if(write(fd1, "*", 1) == -1) {
        perror("pipe write init error");
        exit(EXIT_FAILURE);
    }

    int size = atoi(argv[2]);

    shm_fd = shm_open(argv[1], O_CREAT | O_RDWR, 0664);
    if(shm_fd < 0) {
        perror("Could not aquire shm");
        return 1;
    }

    ftruncate(shm_fd, size * sizeof(char));

    shared_char = (char*)mmap(0, size * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shared_char == MAP_FAILED) {
        perror("Could not map the shared memory");
        close(shm_fd);
        exit(2);
    }
    
    close(shm_fd);

    // Write from a to z until size is reached
    int count = 0;
    char letter = 'a';

    printf("proc1: Reached loop\n");
    while(count < size) {
        // Consume 1 permission each time, essentially waiting for process 2
        //printf("proc1: Reached read\n");
        if(read(fd1, message, 1) == -1) {
            perror("read pipe error");
            exit(1);
        }

        if(letter > 'z') {
            letter = 'a';
        }
        //printf("Reached shared letter write\n");
        shared_char[count] = letter;
        ++letter;
        ++count;
        // Give 1 permission to semaphore 1, allowing process 2 iteration
        //printf("proc1: Reached write\n");
        if(write(fd, "*", 1) == -1) {
            perror("write pipe error");
            exit(1);
        }
    }

    munmap(shared_char, size * sizeof(char));
    shared_char = NULL;

    shm_unlink(argv[1]);

    // Close the named pipes
    close(fd);
    close(fd1);


    // Remove the named pipes
    unlink(FIFO_NAME);
    unlink(FIFO_NAME1);

    return 0;
}