#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>

#define SEM_NAME "/file_semaphore"
#define SEM_NAME1 "/file_semaphore1"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 1;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("Failed to get file size");
        close(fd);
        return 1;
    }

    size_t file_size = file_stat.st_size;
    unsigned char* file_content = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("Failed to map file into memory");
        close(fd);
        return 1;
    }

    close(fd);

    sem_t* semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (semaphore == SEM_FAILED) {
        perror("Failed to create semaphore");
        munmap(file_content, file_size);
        return 1;
    }
    sem_t* semaphore1 = sem_open(SEM_NAME1, O_CREAT, 0666, 0);
    if (semaphore1 == SEM_FAILED) {
        perror("Failed to create semaphore");
        munmap(file_content, file_size);
        return 1;
    }

    printf("File content: ");
    for (size_t i = 0; i < file_size - 1; ++i) {
        sem_wait(semaphore1);
        printf("%c", file_content[i]);
        sem_post(semaphore);
    }
    printf("\n");

    sem_close(semaphore);
    sem_unlink(SEM_NAME);
    munmap(file_content, file_size);

    return 0;
}
