#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/wait.h>

/*======================================================*/

void print_argv(char **argv, char *label){
  printf("\n");
  int j=0;
  while(argv[j] != NULL) {
    j++;
  }
}


/*======================================================*/

char *sh_read_line(void){
  char *line = NULL;
  size_t bufsize = 0; // donc getline realise l'allocation
  getline(&line, &bufsize, stdin);
  return line;
}

/*======================================================*/

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\n"

char **sh_split_line(char *line){
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;
  
  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
  
  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;
    
    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }   
    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/*======================================================*/

int sh_execute(char **args,char **env){      //je rajoute en argument **env qui est le tableau des elements de ma variable d'environnement
  char str[1000]; 
  int i = 0;
  int j = 0;
  int cpt = 1;
  int cpt2 = 0;
  char *f = malloc(sizeof(char)*1000);

  
  while(args[i] != NULL){
   while(env[j] != NULL) {
      if((strcasestr(args[i],env[j])) != NULL) {
			printf("Vas travailler !\n");
			return 0;
      }
      j++;
    }
    i++;
    j=0;
  }
  
  execvp(args[0],args);
  return 0;
}

/*======================================================*/


void newf(char **args){
	int cpt = 1;
	char *t = malloc(sizeof(char)*1000);
   	strcpy(t,getenv("FORBIDDEN"));
   	while(args[cpt] != NULL) {
   		sprintf(t,"%s %s",t,args[cpt]);
   		cpt++;
   	}
		setenv("FORBIDDEN",t,1);
   }


void rmf(char **args){
  		int cpt = 1;
  		int cpt2 = 0;
  		char *t = malloc(sizeof(char)*1000);
		strcpy(t,getenv("FORBIDDEN"));
  		char **ts = sh_split_line(t);
  		char *result = malloc(sizeof(char)*1000);
  		
  		
   	while(args[cpt] != NULL) {
   		while(ts[cpt2] != NULL){
   			if(strcasecmp(args[cpt],ts[cpt2]) !=  0) {
   				sprintf(result,"%s %s",result,ts[cpt2]);
   			}
	   		cpt2++;	
   		}
   		cpt++;
   		cpt2 = 0;
   	}
   	setenv("FORBIDDEN",result,1);
}


void supprCtrlC (int sig){
	printf("Tu fais quoi la ?\n");
}

void eliminer(int sig){
/* Handler du signal SIGCHLD */
    int exit_cond;
    pid_t pid;
    printf("Attente de la terminaison du fils...\n"); 
    pid = wait(&exit_cond);
    #if 0
/* Utilisation alternative d’une gestion non bloquante du wait */ 
    while(waitpid (-1, &sexit_cond , WNOHANG));
    #endif
    if (WIFEXITED(exit_cond))
        printf("Le fils %d s’est termine correctement : %d\n", pid, WEXITSTATUS (exit_cond));
    else
        printf("Le fils %d s’est mal termine : %d\n", pid, WTERMSIG(exit_cond));
}

/*======================================================*/

void sh_loop(void){
  char *prompt = "l3miage shell > ";
  char *line;
  char **args;
  int status=0;
  pid_t pid;
  char *f = malloc(sizeof(char)*1000);
  char **fs;



  if(getenv("FORBIDDEN") != NULL) {
  	 strcpy(f,getenv("FORBIDDEN")); // DIFF surtout pas =
    printf("importation de la variable FORBIDDEN : %s\n",f);

  }
  else {
    setenv("FORBIDDEN","",1);
    printf("creation de la variable FORBIDDEN : %s\n",f);
  }
  
  struct sigaction action ;
  memset (& action , 0, sizeof (action));
  action.sa_handler = eliminer; 
  sigaction ( SIGCHLD , &action , NULL );
 
       	
  do {

  	strcpy(f,getenv("FORBIDDEN"));
   
   fs = sh_split_line(f);

   printf("%s",prompt);
   fflush(stdout);
    
   line = sh_read_line();    
   args = sh_split_line(line);

		   
   
   	if(strcmp(args[0], "exit") == 0){
    		printf("Tu degages !\n");
    		exit(0);
   	}
   
   	if(strcmp(args[0], "newf") == 0){
    		newf(args);
   	}
   
   	if(strcmp(args[0], "rmf") == 0){
    		rmf(args);
   	}
   	else 
   	{
   		
    	pid_t pid= fork();
    	
    	if(pid == 0) {
      	status = sh_execute(args,fs);
      }
 		}
  } while(status == 0);  
}



int main(int argc, char * argv[]){
	
  struct sigaction action ;
  memset (&action , 0, sizeof(action));
  action.sa_handler = supprCtrlC ;
  action.sa_flags = 0;
  sigaction (SIGINT, &action, NULL);
	


	

  // Init : Load config files, if any

  // Interp : Run Command loop
  sh_loop();
  
  // Termin : Perform any shutdown / cleanup
  
  
  return EXIT_SUCCESS;
}









