#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>


#define BUFFER_SIZE 1024

// I AM NOT A FOOTBALL FAN, BUT I KNOW THIS IS MESSY
// Program appears to start behaving very badly for inputs greater than 12 steps. Having an exponential growth and creating so many processes, 
// I think it's fair to say this is to be expected. 
// The failing reason appears to be a pid lower than 0, meaning it fails to create any new processes after about few thousands.

// I need to create a recursive function which will navigate through the entire process tree hierarchy and not miss a single process. While loop with wait() simply does
// not work as it will only look one level deep into the tree from where it's at. I need to find a way to get the children's pid knowing current node's pid
// 3 hours later, that idea, while good at first glance, is impossiblle to implement without a way for a parent process to get it's children pids. 
// Being limited to only functions up to lab 6, exit and wait seemed to be the way, but I settled for open, write and read because, again, without a way to find ALL
// children processes of a parent, iterate through each of them, and then do that recursively until there are no children, there is no good use. This file descriptor
// implementation however, while it works correctly, that might not always be the case. It relies on parent process being last to execute after all of its children 
// finished, otherwise it will read incorrect number of chars from file. So, very large input numbers will definitely break it, not only the write and read will take
// forever, but also the sleep(1) function will not be enough to wait for thousands or even more children processes to terminate. My idea is the following here:
// The program works, not very fast but can trade speed for correctness, and the use of mathematics to compute the actual number way faster can serve as a nice
// double-check for the primitive way of counting(use formula command).
// Formula seems impossible to crack for values ranging 1<skip<steps, also the number of processes increases and exceeds allowable system limits WAY before long read
// and writes from file become a noticeable hinderance

int main(int argc, char **argv) {
    int pid = 0, steps, skip;

    int count = 0;
    int fd;

    if (argc == 4 && !strcmp(argv[1], "formula")) {
        sscanf(argv[2], "%d", &steps);
        sscanf(argv[3], "%d", &skip);
        int n = -1;

        if(steps == 0 || skip == 0) {
            n = 1;
        } else if(skip >= steps) {
            n = pow(2, steps);
        } else if(skip == 1) {
            n = steps + 1;
        } else if(steps == skip + 1) { //The rest of cases when 1<skip<steps I can't figure out their formula
            n = 0.75 * pow(2, steps);
        }

        if(n > 0) {
            printf("Total number of processes(computed with formula): %d\n", n);
            exit(0);
        } else {
            printf("Unknown formula case(cases when 1<skip<steps are hard to determine)\n");
            exit(-1);
        }

    } else if(!strcmp(argv[1], "formula")) {
        printf("Usage: %s formula <loop_steps> <skip_step>", argv[0]);
        exit(1);
    }

    // Open the file for writing
    fd = open("shared.txt", O_RDWR | O_CREAT | O_TRUNC, 0644); //read and write permission for owner, read-only permission for user and other
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    if (argc != 3) {
        printf("Usage: %s <loop_steps> <skip_step>", argv[0]);
        exit(1);
    }
    sscanf(argv[1], "%d", &steps);
    sscanf(argv[2], "%d", &skip);
    for (int i = 1; i <= steps; i++) {
        pid = fork();
        //wait(NULL)
        if (pid == 0 && i % skip == 0)
        {
            break;
        }
    }
    if(pid < 0) {
        printf("Error!");
        exit(-1);
    }
    if(getpgrp() != getpid()) { //prints a character marking both children and grandchildren, grandgrandchildren and so on, which means nothing will be missed
            if (write(fd, "*", 1) == -1) { 
            perror("Error writing to file");
            close(fd);
            return 1;
        }
    }

    if(getpgrp() == getpid()) { // Only root process will reach here
        while(wait(NULL) > 0) { // Wait for children to finish before attempting to read
        } 
        sleep(0.02 * steps + 1); // Make sure every grandchild and so on are also finished. If the input parameters are too large, this will need to also increase
        // Read from the file the number of characters printed
        char ch;
        lseek(fd, 0, SEEK_SET); // Move file pointer to beginning of file
        while(read(fd, &ch, 1) > 0) { // I know this approach gets slower for large files, but the number of processes will increase way before that and not even allow it
            //printf("Read a char\n");
            count++;
        }
        count++;// Add 1 to account for root process
        printf("Number of processes created: %d\n", count);
    }

    // Close the file
    close(fd);
}
