#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
 
#define BUF_SIZE 8192
 
int main(int argc, char* argv[]) {
  int input_fd, output_fd;  /* Input and output file descriptors */
  ssize_t ret_in, ret_out;  /* Number of bytes returned by read() and write() */
  char buffer[BUF_SIZE];    /* Character buffer */
  const char delimiters[] = " \t\r\n";
  char *ptr;
  char *token;
  char *variable;
  int varsDetected = 0;
  float x;
  char line[BUF_SIZE];
  int s_len;

  if(argc != 3){
    printf ("Usage: parseIn file1 file2");
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
    token = (char *) strtok(buffer, delimiters);
    while ((token!=NULL)&&(strlen(token)>0)&&(varsDetected>=0)){
      variable = strtok(NULL, delimiters);
      if (strncmp(variable,"functions",9)==0)
        varsDetected = -1;
      if (varsDetected==1) {
        x = atof(token);
        memset(line,0,BUF_SIZE);
        if (strncmp(variable,"wt",2)==0)
          sprintf(line,"%09.4f %s\n", x*6.366, variable);
        else if (strncmp(variable,"kd",2)==0)
          sprintf(line,"%09.4f %s\n", x*0.565, variable);
        else if (strncmp(variable,"tu",2)==0)
          sprintf(line,"%06.4f %s\n", x*0.02089, variable);
        else if (strncmp(variable,"cw",2)==0)
          sprintf(line,"%09.4f %s\n", x*3.28084, variable);
        else if (strncmp(variable,"ch",2)==0)
          sprintf(line,"%09.4f %s\n", x*3.28084, variable);
        else if (strncmp(variable,"pw",2)==0)
          sprintf(line,"%09.4f %s\n", x*3.28084, variable);
        else if (strncmp(variable,"ph",2)==0)
          sprintf(line,"%09.4f %s\n", x*3.28084, variable);
        else if (strncmp(variable,"uv",2)==0)
          sprintf(line,"%09.4f %s\n", x/1233.481, variable);
        else
          sprintf(line,"%09.4f %s\n", x, variable);
        s_len = strlen(line);
        ret_out = write (output_fd, &line, (ssize_t) s_len);
        if(ret_out < 0) {
          /* Write error */
          perror("write");
          return 4;
        }
      }
      if (strncmp(variable,"variables",9)==0)
        varsDetected = 1;
      token = strtok(NULL, delimiters);
    }
  }					     
  /* Close file descriptors */
  close (input_fd);
  close (output_fd);

  return (EXIT_SUCCESS);
}
