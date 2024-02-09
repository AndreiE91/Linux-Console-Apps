#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int fd;
    off_t size, i;
    char *data = NULL;

    if(argc != 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if(fd == -1) {
        perror("Could not open input file");
        exit(1);
    }

    size = lseek(fd, 0, SEEK_END);
    //printf("Size = %ld\n", size);
    lseek(fd, 0, SEEK_SET);

    data = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(data == MAP_FAILED) {
        perror("Could not map file");
        close(fd);
        exit(1);
    }

    close(fd);

    for(i = size - 1; i >= 0; --i) {
        printf("%c", data[i]);
    }
    printf("\n");

    munmap(data, size);

    return 0;
}