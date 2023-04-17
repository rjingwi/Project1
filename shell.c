#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128



char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;

int status = 0;

void handler(int signum){
  if(getpid()==0){
    exit(0);
  }
}

void handler2(int signum){
  if(getpid() ==0){
    exit(0);
  }
}

int main() {
    
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];

    signal(SIGINT,handler);
    signal(SIGALRM,handler2);
    	
    while (true) {
      
        do{ 
            size_t cwdsize = 500;
            char cwd[cwdsize];
            char* cwdp = getcwd(cwd,cwdsize);

            printf("%s%s",cwdp,prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }


        char* pch = strtok(command_line,delimiters);
        int i = 0;
        while(pch != NULL){
          arguments[i] = pch;
          pch = strtok(NULL," ");
          i++;
        }

        if (i>1){
          arguments[i-1][strlen(arguments[i-1])-1] = 0;
        }

        
        char* command = arguments[0];
        char* last_arg = arguments[i-1];
        int args_length = i;
        bool bg_process = false;

        if(strcmp(last_arg,"&")== 0){
          bg_process = true;
          arguments[i-1] = NULL;
          last_arg = arguments[i-2];
        }
              
        if(strcmp(command,"cd")== 0){
          chdir(arguments[1]);
        }
        else if(strcmp(command,"pwd") == 0){
          size_t cwdsize = 500;
          char cwd[cwdsize];
          char* cwdp = getcwd(cwd,cwdsize);
          printf("%s\n",cwdp);
        }
        else if(strcmp(command,"echo") == 0){
          for(i =1;i<args_length;i++){
            if(i!= args_length-1){
              if(arguments[i][0]=='$'){
                arguments[i]++;
                printf("%s ",getenv(arguments[i]));
              }
              else{
                printf("%s ",arguments[i]);
              }
            }
            else{
              if(arguments[i][0]=='$'){
                arguments[i]++;
                printf("%s\n",getenv(arguments[i]));
              }
              else{
                printf("%s\n",arguments[i]);
              }
            }
          }
        }
        else if(strcmp(command,"exit") == 0){
          exit(0);
        }
        else if(strcmp(command,"env") == 0){
          for(i=0;environ[i]!=NULL;i++){
            printf("%s\n",environ[i]);
          }
        }
        else if(strcmp(command,"setenv") == 0){
          char* definition = arguments[1];
          char* var_name = strtok(definition,"=");
          char* value = strtok(NULL,"=");
          setenv(var_name,value,1);
        }
        else{
          pid_t pid;
          pid = fork();
          if(pid == 0){
            arguments[args_length]= NULL;
            alarm(10);
            execvp(command,arguments);
            exit(0);
          }
          else{
            if (!bg_process){
              wait(&status);
            }
          }
        }

      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}