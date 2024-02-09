#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Obtain file size
    off_t size = lseek(fd, 0, SEEK_END);
    
    // Move pointer back to beginning
    lseek(fd, 0, SEEK_SET);

    // Map the file into memory
    char* file_content = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("Failed to map file into memory");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Remove vowels from the file content
    int vowel_count = 0;
    for (int i = 0; i < size; ++i) {
        switch (file_content[i]) { // Employ "fall through" switch 
            case 'a':
            case 'e':
            case 'i':
            case 'o':
            case 'u':
            case 'A':
            case 'E':
            case 'I':
            case 'O':
            case 'U':
                ++vowel_count;
                file_content[i] = -1;// Mark deleted characters with a character which a regular file won't contain
                break;
        }
    }
    // Shift some of the letters by removing marker characters in between them
    for (off_t i = 0; i < size; ++i) {
    if (file_content[i] == -1) {
        int j = i;
        while (j < size - 1) {
            file_content[j] = file_content[j + 1];
            ++j;
        }
    }
}



    // Add necessary number of dots at the end to maintain file size
    //printf("Size = %ld, vowel_count = %d\n", size, vowel_count);
    for (int i = size - vowel_count - 1; i < size - 1; ++i) {
        file_content[i] = '.';
        //printf("DOT\n");
    }

    // Unmap the file from memory
    if (munmap(file_content, size) == -1) {
        perror("Failed to unmap file from memory");
    }

    close(fd);

    printf("Vowels removed and file size maintained successfully!\n");

    return 0;
}
