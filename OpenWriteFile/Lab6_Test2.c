#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int fdR, fdW;
char c;
rd_wr()
{
    for (;;)
    {
        if (read(fdR, &c, 1) != 1)
            return;
        write(fdW, &c, 1);
    }
}
int main(int argc, char *argv[])
{
    if (argc != 3)
        exit(1);
    if ((fdR = open(argv[1], O_RDONLY)) < 0)
    {
        perror("The error open");
        exit(1);
    }
    if ((fdW = creat(argv[2], 0600)) < 0)
    {
        perror("The error create");
        exit(1);
    }
    fork();
    rd_wr();
    exit(0);
}
/*
If this program is run on files with increasingly larger sizes, the program's performance may be affected by several factors, including the size of the files, t
he available memory, and the disk I/O performance.

When the program reads data from the input file using the read system call, it reads one byte at a time. This means that the program may need to make many system calls 
to read a large file, which can be slow. Additionally, the program writes one byte at a time to the output file using the write system call, which can also be slow for 
large files.

As the size of the input file increases, the program may use more memory to buffer the input and output data, which can cause the program to slow down or run out of 
memory if the available memory is limited.

Finally, the performance of the program may be affected by the disk I/O performance, which can vary depending on the speed and workload of the disk subsystem. 
For example, if the disk subsystem is busy with other I/O operations, the program's performance may be slower.

In general, for larger files, it is more efficient to read and write data in larger chunks rather than one byte at a time. 
This can be done by using larger buffer sizes and by optimizing the I/O operations. Additionally, using parallel processing techniques, 
such as threading or multiprocessing, may also improve the program's performance.
*/