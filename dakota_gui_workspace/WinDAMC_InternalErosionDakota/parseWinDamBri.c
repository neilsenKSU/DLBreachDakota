#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
 
#define BUF_SIZE 16384
 
int main(int argc, char* argv[]) {
  int input_fd, output_fd;  /* Input and output file descriptors */
  ssize_t ret_in, ret_out;  /* Number of bytes returned by read() and write() */
  char buffer[BUF_SIZE];    /* Character buffer */
  const char delimiters[] = "\r\n";
  char *ptr;
  char *token;
  char *variable;
  char *linePtr;
  int varsDetected = 0;
  float x, low, high;
  char line[BUF_SIZE];
  int s_len;
  int skip_lines = 5;

  low = 0.0;
  high = 0.0;
  x = 0.0;

  if(argc != 3){
    printf ("Usage: parseBri file1 file2");
    return 1;
  }

  /* Create input file descriptor */
  input_fd = open (argv[1], O_RDONLY);
  if (input_fd == -1) {
    perror ("open");
    return 2;
  }
				 
  /* Create output file descriptor */
  output_fd = open(argv[2], O_WRONLY | O_CREAT, 0644);
  if(output_fd == -1){
    perror("open");
    return 3;
  }
					 
  /* Parse process */
  memset(line,0,BUF_SIZE);
  memset(buffer,0,BUF_SIZE);
  while((ret_in = read (input_fd, &buffer, BUF_SIZE)) > 0){
    linePtr = (char *) strtok(buffer, delimiters);
    while ((linePtr!=NULL)&&(varsDetected==0)){
      if (strncmp(linePtr,"     Time", 9)==0) {
  	skip_lines = 2;
      }
      if (skip_lines <= 0) {
        strncpy(line, linePtr+310, 10);
        low = atof(line);
        memset(line,0,BUF_SIZE);
        strncpy(line, linePtr+320, 10);
        high = atof(line);
        memset(line,0,BUF_SIZE);
        if ((high-low)>x) {
          x = high-low;
        }
      } else {
        skip_lines--;
      }
      linePtr = (char *) strtok(NULL, delimiters);
    }
  }
  x *= 0.3048; // convert ft to m
  sprintf(line,"%9.2f f1\n", x);
  s_len = strlen(line);
  varsDetected = 1;
  ret_out = write (output_fd, &line, (ssize_t) s_len);
  if(ret_out < 0) {
    /* Write error */
    perror("write");
    return 4;
  }
  /* Close file descriptors */
  close (input_fd);
  close (output_fd);

  return (EXIT_SUCCESS);
}

