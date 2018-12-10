#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
  
#define PORT 8090 
  
int main() 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char str[100]; 
  
    printf("\nPlease enter the string:"); 
    scanf("%[^\n]s", str); 
  
    char buffer[1024] = { 0 }; 
  
    // Creating socket 
    if ((sock = socket(AF_INET, 
                       SOCK_STREAM, 0)) 
        < 0) { 
        printf("\n Socket not created \n"); 
        return -1; 
    } 
  
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
  
  
    if (inet_pton(AF_INET, "127.0.0.1", 
                  &serv_addr.sin_addr) 
        <= 0) { 
        printf("\nAddress not supported \n"); 
        return -1; 
    } 
  
    // connect the socket 
    if (connect(sock, (struct sockaddr*)&serv_addr, 
                sizeof(serv_addr)) 
        < 0) { 
        printf("\nno connection  \n"); 
        return -1; 
    } 
  
    int l = strlen(str); 
  
    // send string to server side 
    send(sock, str, sizeof(str), 0); 
  
    // read string sent by server 
    valread = read(sock, str, l); 
  
    printf("%s\n", str); 
  
    return 0; 
} 
