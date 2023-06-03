#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

// /home/osexam/LabSim/TEST
int main(int argc, char** argv)
{
    if(argc != 2) {
        printf("Usage: %s <file path>\n", argv[0]);
        exit(-1);
    }

    int fd = open(argv[1], O_RDWR);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    printf("TEST file's size: %d\n", size);
    int rem = size % 16;
    size -= rem;
    ftruncate(fd, size);
    printf("New TEST file's size: %d\n", size);

    // Navigate to beginning of second quarter of file
    lseek(fd, size / 4, SEEK_SET);
    char* buf = malloc(size / 4 + 1);
    read(fd, buf, size / 4);
    buf[size / 4] = '\0'; // Null terminate buffer
    printf("%s\n", buf);
    free(buf);

    int* bytes;
    if ((bytes = mmap(0, size / 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (size / 4) * 3)) == MAP_FAILED) {
        perror("Can't map\n");
        exit(1);
    } else {
        printf("Successfully mapped last quarter of file into memory!\n");
    }

    int sum = 0;
    for (int i = 0; i < size / 4; ++i) {
        sum += bytes[i];
    }
    printf("Sum of last quarter of file treated as 4-byte integers: %d\n", sum);
    close(fd);
    return 0;
}