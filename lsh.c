/* 
 * Main source code file for lsh shell program
 *
 * You are free to add functions to this file.
 * If you want to add functions in a separate file 
 * you will need to modify Makefile to compile
 * your additional functions.
 *
 * Add appropriate comments in your code to make it
 * easier for us while grading your assignment.
 *
 * Submit the entire lab1 folder as a tar archive (.tgz).
 * Command to create submission archive: 
      $> tar cvf lab1.tgz lab1/
 *
 * All the best 
 */


#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

/*
 * Function declarations
 */

void PrintCommand(int, Command *);
void PrintPgm(Pgm *);
void RunCommand(Pgm *, char *,char *,int,int);
void stripwhite(char *);
void no_zombies();

/* When non-zero, this global means the user is done using this program. */
int done = 0;

/*
 * Name: main
 *
 * Description: Gets the ball rolling...
 *
 */

void no_zombies(){
	waitpid(-1, NULL, WNOHANG);
}

int main(void)
{
  	Command cmd;
	int n;
	//signal(SIGINT, SIG_IGN);
	//signal(SIGCHLD, no_zombies);

  while (!done) {

	//om pgm->next == NULL inge fler kommandon att exekvera
	//annars ska stdout från ett kopplas till stdin för ett annat 

    char *line;
    line = readline("> ");

    if (!line) {
      /* Encountered EOF at top level */
      done = 1;
    }
    else {
      /*
       * Remove leading and trailing whitespace from the line
       * Then, if there is anything left, add it to the history list
       * and execute it.
       */
      stripwhite(line);

      if(*line) {
        add_history(line);
        /* execute it */
        n = parse(line, &cmd);
        PrintCommand(n, &cmd);
		RunCommand(cmd.pgm,cmd.rstdout,cmd.rstdin,cmd.bakground,0);
      }
    }
    
    if(line) {
      free(line);
    }
  }
  return 0;

}
/*
 * Name: RunCommand
 *
 *
 */
void 
RunCommand(Pgm *p, char *out, char *in, int bg,int n){
	int pipefd[2];
	printf("%d \n",p->next);
	if(p->next != NULL){
		printf("%d \n",123);
		pipe(pipefd);
		printf("%d \n",324);
	}	
	pid_t pid; 
	pid = fork();

	if(pid<0){
		printf("error \n");
	}
	else if(pid == 0){
		/*child*/

		if(out != NULL){
		
			int fd1 = creat(out , 0644) ;
        	dup2(fd1, STDOUT_FILENO);
        	close(fd1);
		}

		if(in != NULL){
		
			int fd2 = open(in , O_RDONLY) ;
        	dup2(fd2, STDIN_FILENO);
        	close(fd2);
		}
		if(n != 0){
			perror("in child");
			close(pipefd[0]);
			dup2(pipefd[1],STDOUT_FILENO);
			close(pipefd[1]);
			//set my out to parents in
			 
		}
		//if bg not set 
		//setpgid(0,0)
		execvp((p->pgmlist)[0],(p->pgmlist));
		if(p->next != NULL){
		//recursion next 
		//öppna en ny in och skicka med som out
		//till nästa runCommand  
		// 	
			RunCommand(p->next,in,in,bg,n+1);
		}
	} 
	else{
		/*parent*/ 	
		//if bg is not set 
		printf("in parent");
		if(p->next != NULL){
			printf("piping");
			close(pipefd[1]);
			dup2(pipefd[0],STDIN_FILENO);
			close(pipefd[0]);
		}
		if (bg==0){
			//not bg
			signal(SIGCHLD, SIG_DFL);
			wait(NULL);
		}
		else{
			signal(SIGCHLD, no_zombies);
		}
		
		//wait(NULL);
	}
}




/*
 * Name: PrintCommand
 *
 * Description: Prints a Command structure as returned by parse on stdout.
 *
 */
void
PrintCommand (int n, Command *cmd)
{
  printf("Parse returned %d:\n", n);
  printf("   stdin : %s\n", cmd->rstdin  ? cmd->rstdin  : "<none>" );
  printf("   stdout: %s\n", cmd->rstdout ? cmd->rstdout : "<none>" );
  printf("   bg    : %s\n", cmd->bakground ? "yes" : "no");
 
 	/*PrintPgm(cmd->pgm);
		check if bg set 
	*/
	
	
			
	
}

/*
 * Name: PrintPgm
 *
 * Description: Prints a list of Pgm:s
 *
 */
void
PrintPgm (Pgm *p)
{
  if (p == NULL) {
    return;
  }
  else {
    char **pl = p->pgmlist;

    /* The list is in reversed order so print
     * it reversed to get right
     */
    PrintPgm(p->next);
    printf("    [");
    while (*pl) {
      printf("%s ", *pl++);
    }
    printf("]\n");
  }
}

/*
 * Name: stripwhite
 *
 * Description: Strip whitespace from the start and end of STRING.
 */
void
stripwhite (char *string)
{
  register int i = 0;

  while (isspace( string[i] )) {
    i++;
  }
  
  if (i) {
    strcpy (string, string + i);
  }

  i = strlen( string ) - 1;
  while (i> 0 && isspace (string[i])) {
    i--;
  }

  string [++i] = '\0';
}
