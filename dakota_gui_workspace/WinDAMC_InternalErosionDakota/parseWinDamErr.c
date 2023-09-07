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
  int input1_fd, input2_fd, output_fd;  /* Input and output file descriptors */
  ssize_t ret_in, ret_out;  /* Number of bytes returned by read() and write() */
  char buffer[BUF_SIZE];    /* Character buffer */
  const char delimiters[] = "\r\n";
  char *ptr;
  char *token;
  char *variable;
  char *linePtr;
  int varsDetected = 0;
  int getHeight = 0;
  int code, collapsed;
  float damHeight = 51.05; /* default height */
  float time, low, high, width, depth, prevWidth, prevDepth;
  float maxBreachFlow, maxBreachTime, maxBreachWidth, maxBreachDepth, collapseTime, bcWidth, bcDepth, acWidth, acDepth, avgErr;
  char line[BUF_SIZE];
  int s_len;
  int skip_lines = 5;

  if(argc != 4){
    printf ("Usage: parseOut file1 file2 file3");
    return 1;
  }
			 
  /* Create file descriptor for input 1 */
  input1_fd = open(argv[1], O_RDONLY);
  if (input1_fd == -1) {
    perror ("open");
    return 2;
  }
  
  /* Create file descriptor for input 2 */
  input2_fd = open(argv[2], O_RDONLY);
  if (input2_fd == -1) {
    perror("open");
    return 3;
  }
				 
  /* Create output file descriptor */
  output_fd = open(argv[3], O_WRONLY | O_CREAT, 0644);
  if(output_fd == -1){
    perror("open");
    return 4;
  }
					 
  /* Parse first input file */
  while((ret_in = read(input1_fd, &buffer, BUF_SIZE)) > 0){
    linePtr = (char *) strtok(buffer, delimiters);
    while ((linePtr!=NULL)&&(varsDetected<3)){
      if (strncmp(linePtr,"Peak Outflow:", 13)==0) {
        strncpy(line, linePtr+45, 10);
        maxBreachFlow = atof(line);
        maxBreachFlow *= 0.028316847; // convert cfs to cms
        memset(line,0,BUF_SIZE);
        strncpy(line, linePtr+61, 10);
        maxBreachTime = atof(line);
        maxBreachTime *= 3600; // convert hrs to secs
        memset(line,0,BUF_SIZE);
        varsDetected += 2;
      } else if (strncmp(linePtr," CRESTPRFL", 10)==0) {
        getHeight = 1;
      } else if (getHeight==1) {
        strncpy(line, linePtr+20, 10);
        damHeight = atof(line);
        varsDetected += 1;
      	getHeight = 0;
      }
      linePtr = (char *) strtok(NULL, delimiters);
    }
  }
  
  /* Initialize variables */
  maxBreachWidth = 0.0;
  maxBreachDepth = 0.0;
  width = 0.0;
  depth = 0.0;
  prevWidth = 0.0;
  prevDepth = 0.0;
  low = 0.0;
  high = 0.0;
  collapsed = 0;
  collapseTime = 0.0;
  bcWidth = 0.0;
  bcDepth = 0.0;
  acWidth = 0.0;
  acDepth = 0.0;
  memset(line,0,BUF_SIZE);
  memset(buffer,0,BUF_SIZE);
  
  /* Parse second input file */
  while((ret_in = read(input2_fd, &buffer, BUF_SIZE)) > 0){
    linePtr = (char *) strtok(buffer, delimiters);
    while ((linePtr!=NULL)){
      if (strncmp(linePtr,"     Time", 9)==0) {
        skip_lines = 2;
      }
      if (skip_lines <= 0) {
        /* Get current time */
        strncpy(line, linePtr, 10);
        time = atof(line);
        memset(line,0,BUF_SIZE);
        /* Calculate width */
        strncpy(line, linePtr+310, 10);
        low = atof(line);
        memset(line,0,BUF_SIZE);
        strncpy(line, linePtr+320, 10);
        high = atof(line);
        memset(line,0,BUF_SIZE);
        prevWidth = width;
        width = high-low;
        if (width>maxBreachWidth) {
          maxBreachWidth = width;
        }
        /* Calculate height */
        strncpy(line, linePtr+40, 10);
        low = atof(line);
        memset(line,0,BUF_SIZE);
        strncpy(line, linePtr+60, 10);
        high = atof(line);
        memset(line,0,BUF_SIZE);
        if (high>damHeight) {
          high = damHeight;
        }
        prevDepth = depth;
        depth = high-low;
        if (depth>maxBreachDepth) {
          maxBreachDepth = depth;
        }
        /* Determine if roof collapsed */
        strncpy(line, linePtr+330, 10);
        code = atoi(line);
        memset(line,0,BUF_SIZE);
        /* Roof collapse */
        if (collapsed<2) {
          if (high==damHeight || collapsed==1) {
            collapseTime = time * 3600;
            bcWidth = prevWidth * 0.3048;
            bcDepth = prevDepth * 0.3048;
            acWidth = width * 0.3048;
            acDepth = depth * 0.3048;
            collapsed = 2;
            varsDetected += 5;
          } else if (code==4 && (width > 2*(damHeight-high))) {
            collapsed = 1;
          }
        }
      } else {
        skip_lines--;
      }
      linePtr = (char *) strtok(NULL, delimiters);
    }
  }
  maxBreachWidth *= 0.3048; // convert ft to m
  maxBreachDepth *= 0.3048; // convert ft to m
  varsDetected += 2;
  
  avgErr = (abs(maxBreachFlow-3313)/3313 + abs(maxBreachTime-3300)/3300 + abs(maxBreachWidth-96.15)/96.15 + abs(maxBreachDepth-24.7)/24.7 + 
    abs(collapseTime-600)/600 + abs(bcWidth-0.46)/0.46 + abs(acWidth-10)/10.0 + abs(bcDepth-0.46)/0.46 + abs(acDepth-10)/10.0)/9 * 100;
  
  /* Output results */
  memset(line,0,BUF_SIZE);
  sprintf(line,"%9.3f f1\n", avgErr);
  s_len = strlen(line);
  ret_out = write (output_fd, &line, (ssize_t) s_len);
  if(ret_out < 0) {
    /* Write error */
    perror("write");
    return 5;
  }
  
  /* Close file descriptors */
  close (input1_fd);
  close (input2_fd);
  close (output_fd);

  return (EXIT_SUCCESS);
}

