#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	int fd;
	off_t size, i;
	char *data = NULL;
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		perror("Could not open input file");
		return 1;
	}
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	data = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(data == (void*)-1) {
		perror("Could not map file");
		close(fd);
		return 1;
	}

    // Initially, the program already printed the contents of the file in reverse due to the way the loop was written.
    // I assume then that what the statement asks is for me to reverse the already reversed contents, such that the file now is 
    // displayed in its actual normal order.
	for(i=0; i< size; i++) {
		printf("%c", data[i]);
	}
	printf("\n");

	munmap(data, size);
	close(fd);

	return 0;
}