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
  float x, y;
  char line[BUF_SIZE];
  int s_len;

  if(argc != 3){
    printf ("Usage: parseOut file1 file2");
    return 1;
  }
			 
  /* Create input file descriptor */
  input_fd = open (argv [1], O_RDONLY);
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
  while((ret_in = read (input_fd, &buffer, BUF_SIZE)) > 0){
    linePtr = (char *) strtok(buffer, delimiters);
    while ((linePtr!=NULL)&&(varsDetected==0)){
      if (strncmp(linePtr,"Peak Outflow:", 13)==0) {
        strncpy(line, linePtr+45, 10);
        x = atof(line);
        x *= 0.028316847; // convert cfs to cms
        strncpy(line, linePtr+61, 10);
        y = atof(line);
	memset(line,0,BUF_SIZE);
        sprintf(line,"%9.2f f1\n%9.3f f2\n", x,y);
	s_len = strlen(line);
        varsDetected = 2;
	ret_out = write (output_fd, &line, (ssize_t) s_len);
        if(ret_out < 0) {
          /* Write error */
          perror("write");
          return 4;
        }
      }
      linePtr = (char *) strtok(NULL, delimiters);
    }
  }					     
  /* Close file descriptors */
  close (input_fd);
  close (output_fd);

  return (EXIT_SUCCESS);
}

