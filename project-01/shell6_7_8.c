#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define DELIMITERS " ,"
#define MAX_STRING_LENGTH 200
#define MAX_T 100




// we will return number of strings
int parse(char* line, char*** tokenized) {

  char* line_buffer;
  int argc = 0;

  line_buffer = (char*) malloc(strlen(line) * sizeof(char));
  strcpy(line_buffer,line);

  /*This line is saving memory for the char
 
  */
  (*tokenized) = (char**) malloc(MAX_T * sizeof(char**));

  //here i am grabbing the first token with strtok
  (*tokenized)[argc++] = strtok(line_buffer, DELIMITERS);

  //we must Run through entire char array and tokenizing the rest.


  // line grabbed the first token the aboved
  while ((((*tokenized)[argc] = strtok(NULL, DELIMITERS)) != NULL) && (argc < MAX_T)){
      ++argc;
  }

  (*tokenized)[argc] = '\0';
  return argc; //returning the total number of strings
}


void execute(char **argv){

	pid_t pid__A; //this is for the child process

	pid_t pidB; //this is for the parent process 


	int pipes[2];

	int x = 0; //iterator

  	int a = 0; //iterator

	int pipes_ = 0; 

	char command[100][100];

	char _arg[100][100];
	
	//WE are checking location of | in the input string
	while(argv[pipes_] != NULL && argv[pipes_][0] != '|'){
		pipes_++;
	}





	//if its  not the valid arguments 
	if(argv[pipes_] == NULL || pipes_ == 0 || argv[pipes_ + 1] == NULL ){
		printf("\nWrong input format:");
		if(argv[pipes_] == NULL){
			printf("\nNo '|'  the character found in argument.\n\n");
		}else if(pipes_ == 0){
			printf("\n'|'this cannnot be the first argument\n\n");
		}else {
			printf("\n'|' this character cannot be last argument.\n\n");
		}
		exit(1);
	}

  	// here we are spliting argv from pivot of '|'.
	while(argv[x][0] != '|'){
        	strcpy(command[x], argv[x]);
        	x++;
	}
    	command[x][0] = '\0';
    	x++;
	
	//we are copying the second half after |
    	while(argv[x] != NULL){
        	strcpy(_arg[x], argv[x]); // was k
        	x++;
        	a++;
	}
	_arg[a][0] = '\0';

	//here we initilize the pipe
	if(pipe(pipes) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}





	//here we are creating fork


	if((pid__A = fork()) < 0){



		printf("The Fork failed.\n");


		exit(1);


	}else if(pid__A == 0){ //the child process


		dup2(pipes[1], STDOUT_FILENO); //replace standard output with pipe[out]

		//closes read pipe in child
		close(pipes[0]);

		//execute execvp and return error if exists
		if(execlp(command[0], *command, NULL) < 0){
			printf("Execlp failed.\n");
			exit(1);
		}else {
			exit(EXIT_SUCCESS);
		}


	}else {

		wait(NULL);

		//here we replace standard input with pipe[in]
		dup2(pipes[0], STDIN_FILENO);

		//here we close other end of pipe in parent
		close(pipes[1]);

		//here we execute execvp and return error if exists
		if(execlp(_arg[0], *_arg, NULL) < 0){
			printf("The Execlp parent failed. \n");
			exit(1);
		}
	}
}


int main(){

	char** argv;

	char input_test[100];

	int count = 0;

	int number_of_strings = 0;

	printf("the min shell >");

	scanf("%[^\n]", input_test);

	number_of_strings = parse(input_test, &argv);

	if(number_of_strings < 3){

		printf("\nNot a valid number of arguments  please retry .\nInput: %d\n", number_of_strings);

		exit(1);

	}else {

		execute(argv);

	}

	return 0;
}
