#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    int shm_fd;
    char *shared_char = NULL;

    shm_fd = shm_open("/myshm", O_RDWR, 0);
    if(shm_fd < 0) {
        perror("Could not acquire shm");
        exit(1);
    }

    shared_char = (char*)mmap(0, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shared_char == MAP_FAILED) {
        perror("COuld not map the shared memory");
        exit(1);
    }

    close(shm_fd);

    printf("found shared_char: %c\n", *shared_char);
    *shared_char = 'X';

    munmap(shared_char, sizeof(char));
    shared_char = NULL;

    return 0;
}