
// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
// PORT # must be changed it not working, 
#define PORT 8080 

char** splitString(char* strings);

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1250] = {0}; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){ 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))){ 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){ 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0){ 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){ 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    
    valread = read( new_socket , buffer, 1024); 
    // printf("%s\n",buffer ); 

    /*Start - Alex*/

    int pipes[2];
    pid_t pid;

    if(pipe(pipes) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if(pid < 0){
        printf("Forking another process failed. . .\n");
        exit(1);
    }else if(pid == 0){ // child processs
        // printf("String passed from client: %s\n", buffer);
        char **c = splitString(buffer);

        // ls -l -a null
        // 0  1    2  3
        for(int i = 0; i < 4; i++){
            printf("%s\n", c[i]);
        }
        dup2(pipes[1], STDOUT_FILENO); //replace standard output with pipe[out]
        close(pipes[0]);
        if(execvp(c[0], c) == -1){
            perror("execvp");
        }   
        exit(0);
    }else{ // parent process
        wait(NULL);
        // vvv read outout from child from pipe[0] vvv
        //here we replace standard input with pipe[in]
		dup2(pipes[0], STDIN_FILENO);
        close(pipes[1]);
        char *result = malloc(1250 * sizeof(char));
        read(pipes[0],result, 1250);
        close(pipes[0]);
        // ^^^ read outout from child from pipe[0] ^^^
        send(new_socket , result , strlen(result) , 0 );  
    }
    /*End - Alex*/
   
    return 0; 
}

char** splitString(char* strings){
    char **commands;
    commands = malloc(10 * sizeof(char*));
    for(int i = 0; i < 10; i++){
        commands[i] = malloc(50 * sizeof(char));
    }
    char *token;
    int count = 0;
    token = strtok(strings, " \n");
    commands[count] = token;
    while(token != NULL){
        count++;
        token = strtok(NULL, " \n");
        commands[count] = token;
        
    }    
    return commands;
}
