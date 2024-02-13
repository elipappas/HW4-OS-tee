// Author: Eli Pappas

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MSGSIZE 1

int main(int argc, char **argv)
{
  // Creating the file pointer which is the file passed into t
  FILE *fp;

  // create the in buffer char array
  char inbuf[MSGSIZE];
  // create the pipe
  int p[2];

  // initialize the child
  pid_t child;
  // initialize the return status
  int status;

  // create the pipe and check to see if created successfully
  if (pipe(p) < 0)
    exit(1);

  // fork to create the child
  child = fork();

  // child case
  if (child == 0)
  {
    close(p[1]); // close the write end of the pipe
    fp = fopen(argv[1], "w"); // open the file with write permission

    size_t bytesRead;

    // loop while the pipe is still being read
    while ((bytesRead = read(p[0], inbuf, MSGSIZE)) != 0)
    {
        // prints the inbuf to the terminal
        printf("%s", inbuf);
        // puts the inbuf into the text file
        fwrite(inbuf, sizeof(char), bytesRead, fp);
    }

    // close the file as we are done writing to it
    fclose(fp);
    // close the read end of the pipe
    close(p[0]);
    // exit
    exit(0); 
    
  }

  // parent case
  else
  {
    close(p[0]); // close the read end of the pipe

    // loops while inbuf is being filled with info from the input file (STDIN_FILENO)
    while (read(STDIN_FILENO, inbuf, MSGSIZE) != 0)
    {
      // write the input inbuf to the pipe to be read by the child 
      write(p[1], inbuf, MSGSIZE);
    }

    // close the write end of the pipe
    close(p[1]);            
    // wait for the child to finish before ending    
    waitpid(child, &status, 0); 
  }
}