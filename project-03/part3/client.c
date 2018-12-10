// server side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#define SA struct sockaddr

#define PORT 6969



#define SIZE 1024
char buf[SIZE];


#define TIME_PORT 9005

int main(int argc, char **argv)
{
  int sockfd;
  int nread;
  struct sockaddr_in serv_addr;
  struct hostent *host;
  uint16_t serv_port;
  char *ip;
  int i;

  if (argc != 3)
  {
    fprintf(stderr,"\nUsage: /.bclient <IP> <PORT>\n\n");
    exit(1);
  }

  ip=argv[1];
  serv_port=atoi(argv[2]);

  // create endpoint 
  if ((sockfd = socket(AF_INET,SOCK_STREAM, 0))< 0) {
    perror(NULL);
    exit(2);
  }

  // connect to server 
  serv_addr.sin_family = AF_INET;
  host = gethostbyname(argv[1]);
  memcpy(&serv_addr.sin_addr.s_addr, host->h_addr, host->h_length);
  serv_addr.sin_port =
            htons(serv_port);
  if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr))< 0) {
    perror(NULL);
    exit(3);
  }

  // transfer data to fserver and prints the output for the sent request
  char str[SIZE];
   printf("\nEnter your Command : ");
  while (fgets(str, SIZE, stdin)!=NULL)
  {

	  if(strncasecmp(str,"Exit",4)==0){
		  printf("\nExiting Client\n");
		  break;
	  }
	  if(strcmp(str,"\n")==0){
		  printf("\nEnter your Command : ");
		  continue;
	  }
	  nread=write(sockfd,str,strlen(str));
	  nread = read(sockfd, buf,SIZE);

	  for (i = 1; i < nread; i++) {
			if (buf[i] == 0)
				buf[i] = '0';
		}
	  printf("\nDServer\t: %s\n",buf);

	  printf("\nEnter your Command : ");
	  memset(str,'\0',strlen(str));
	  memset(buf,'\0',strlen(buf));
  }
  close(sockfd);
  exit(0);
}
