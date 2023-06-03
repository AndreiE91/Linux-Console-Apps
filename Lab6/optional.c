#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <string.h>

int main(int argc, char **argv)
{
  int pid, steps, skip;
  int pipefd[2];
  int count = 1; // Count of processes including parent
  if (argc == 4 && !strcmp(argv[1], "formula")) {
    sscanf(argv[2], "%d", &steps);
    sscanf(argv[3], "%d", &skip);
    int n = 0;
    for (int i = skip; i <= steps; i += skip) {
        n += pow(2, steps - i);
    }
    n += 1; // add 1 for parent process
    printf("Total number of processes(computed with formula): %d\n", n);
    exit(0);

  } else if(!strcmp(argv[1], "formula")) {
    printf("Usage: %s formula <loop_steps> <skip_step>", argv[0]);
    exit(1);
  }

  if (argc != 3) {
    printf("Usage: %s <loop_steps> <skip_step>", argv[0]);
    exit(1);
  }
  sscanf(argv[1], "%d", &steps);
  sscanf(argv[2], "%d", &skip);
  
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(1);
  }
  
  for (int i=1; i<=steps; i++) {
    pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(1);
    }
    if (pid == 0) { // child process
      if (i % skip == 0) {
        close(pipefd[0]); // close read end of pipe
        write(pipefd[1], &count, sizeof(count)); // write count to pipe
        close(pipefd[1]); // close write end of pipe
      }
      exit(0);
    } else { // parent process
      if (i % skip == 0) {
        count++; // increment count
      }
    }
  }
  
  if (pid != 0) { // only the parent process should print the count
    for (int i=1; i<steps/skip+1; i++) {
      close(pipefd[1]); // close write end of pipe
      int child_count;
      read(pipefd[0], &child_count, sizeof(child_count)); // read count from pipe
      close(pipefd[0]); // close read end of pipe
      count += child_count;
    }
    printf("Total number of processes(computed with process counting): %d\n", count);
  }
  
  return 0;
}
