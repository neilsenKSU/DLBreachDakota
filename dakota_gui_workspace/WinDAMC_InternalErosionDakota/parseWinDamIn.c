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

        /* length: _m -> ft | .ft -> m */
        if (strncmp(variable,"l_",2)==0)
          sprintf(line,"%09.4f %s\n", x*3.28084, variable);
        else if (strncmp(variable,"l.",2)==0)
          sprintf(line,"%09.4f %s\n", x/3.28084, variable);
        /* length2: _cm -> in | .in -> _cm */
        else if (strncmp(variable,"l2_",3)==0)
          sprintf(line,"%09.4f %s\n", x/2.54, variable);
        else if (strncmp(variable,"l2.",3)==0)
          sprintf(line,"%09.4f %s\n", x*2.54, variable);
        /* area: _m2 -> ac | .ac -> m2 */
        else if (strncmp(variable,"a_",2)==0)
          sprintf(line,"%09.4f %s\n", x/4046.86, variable);
        else if (strncmp(variable,"a.",2)==0)
          sprintf(line,"%09.4f %s\n", x*4046.86, variable);
        /* area2: _m2 -> ft2 | .ft2 -> m2 */
        else if (strncmp(variable,"a2_",3)==0)
          sprintf(line,"%09.4f %s\n", x*10.7639, variable);
        else if (strncmp(variable,"a2.",3)==0)
          sprintf(line,"%09.4f %s\n", x/10.7639, variable);
        /* volume: _m3 -> ac*ft | .ac*ft -> m3 */
        else if (strncmp(variable,"v_",2)==0)
          sprintf(line,"%09.4f %s\n", x/1233.48, variable);
        else if (strncmp(variable,"v.",2)==0)
          sprintf(line,"%09.4f %s\n", x*1233.48, variable);
        /* flow rate: _m3/s -> ft3/s | .ft3/s -> m3/s */
        else if (strncmp(variable,"f_",2)==0)
          sprintf(line,"%09.4f %s\n", x*35.3147, variable);
        else if (strncmp(variable,"f.",2)==0)
          sprintf(line,"%09.4f %s\n", x/35.3147, variable);
        /* erodibility: _cm3/N*s -> ft3/lb*hr | .ft3/lb*hr -> cm3/N*s */
        else if (strncmp(variable,"e_",2)==0)
          sprintf(line,"%09.4f %s\n", x/1.768, variable);
        else if (strncmp(variable,"e.",2)==0)
          sprintf(line,"%09.4f %s\n", x*1.768, variable);
        /* time: _s -> hr | .hr -> s */
        else if (strncmp(variable,"t_",2)==0)
          sprintf(line,"%09.4f %s\n", x*60, variable);
        else if (strncmp(variable,"t.",2)==0)
          sprintf(line,"%09.4f %s\n", x/60.0, variable);
        /* density: _kN/m3 -> lb/ft3 | .lb/ft3 -> kN/m3 */
        else if (strncmp(variable,"d_",2)==0)
          sprintf(line,"%09.4f %s\n", x*6.366, variable);
        else if (strncmp(variable,"d.",2)==0)
          sprintf(line,"%09.4f %s\n", x/6.366, variable);
        /* pressure: _N/m2 -> lb/ft2 | _lb/ft2 -> N/m2 */
        else if (strncmp(variable,"p_",2)==0)
          sprintf(line,"%09.4f %s\n", x/47.88, variable);
        else if (strncmp(variable,"p.",2)==0)
          sprintf(line,"%09.4f %s\n", x*47.88, variable);
        /* uncoverted */
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
