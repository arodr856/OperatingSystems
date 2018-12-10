// server side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 1024
char buf[SIZE];
#define BS 128

#define TIME_PORT 9005
int track,sec;
int fd;
char diskfile[SIZE];
char reader[BS+1];

// structure that defines the block of 128 characters
struct block{
	char val[BS];
};
struct block * blkmap;

int main(int argc, char *argv[])
{
  int sockfd, client_sockfd;
  int nread;
  socklen_t len;
  struct sockaddr_in serv_addr,client_addr;
  uint16_t serv_port;

  int i,j,result,tk,sc,inlen,num;
  char cmd;
  char *bmap,*info;

 // char *cmdError="Command Error\n\n Use Format:\n\t1. I\n\t2. R <track #> <sector #>\n\t3. W <track #> <sector #> <InputLength> <data>\n\t4. Exit";
   char *cmdError;
   cmdError=(char *)malloc(sizeof(char)*SIZE);
   info=(char *)malloc(sizeof(char)*SIZE);
  strcpy(cmdError,"Command Error\n\n Use Format:\n\t1. I\n\t2. R <track #> <sector #>\n\t3. W <track #> <sector #> <InputLength> <data>\n\t4. Exit");
  off_t readfrm=0,writeto=0;
  int totblk=0,bin=0;

// checking for the Command Line Arguments
  if((argc!=5))
  {
	  fprintf(stderr,"\nUsage: ./dserver <PORT> <Track> <Sectors> <filename> \n\n");
	     exit(1);
  }
  serv_port=atoi(argv[1]);
  track=atoi(argv[2]);
  sec=atoi(argv[3]);
  strncpy(diskfile, argv[4],SIZE-1);
  long int fsize = track*sec*BS;
  totblk = track*sec;

  printf("\n Port\t: %d",serv_port);
  printf("\n File\t: %s",diskfile);
  printf("\n Track\t: %d",track);
  printf("\n Sector\t: %d",sec);
  printf("\n Blocks\t: %d",totblk);
  printf("\n FSize\t: %ld",fsize);


  //creating a file based on the given argument
  fd = open(diskfile, O_RDWR | O_CREAT, (mode_t)0600);
      if (fd == -1) {
    	  perror("Error opening file for writing");
    	  exit(EXIT_FAILURE);
      }
      // Using lseek to span the file to the size of fsize
      result = lseek(fd, fsize - 1, SEEK_SET);
      	if (result == -1) {
      		close(fd);
      		perror("Error calling lseek() to 'stretch' the file");
      		exit(EXIT_FAILURE);
      	}
      	//Writing a last byte of the file
      	result = write(fd, "", 1);
      	if (result != 1) {
      		close(fd);
      		perror("Error writing last byte of the file");
      		exit(EXIT_FAILURE);
      	}
      	lseek(fd, 0, SEEK_SET);



  // create endpoint 
  if ((sockfd = socket(AF_INET,SOCK_STREAM, 0))< 0)
  {
    perror(NULL);
    exit(2);
  }


  // bind address 
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(serv_port);


  if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror(NULL);
    exit(3);
  }

  // specify queue 
  listen(sockfd, 5);

  for (;;) {
	  printf("\nListening socket for Incoming Connections ..\n");
    len = sizeof(client_addr);
    client_sockfd = accept(sockfd,(struct sockaddr*)&client_addr,&len);
    printf("\nAccepting Connection Successful..\n");
    if (client_sockfd == -1) {
      perror(NULL);
      continue;
    }

    printf("\nDserver waiting to read from new Connection...\n");

   // start reading from the client socket.
   // here the client starts sending commands to the disk server
   while(1){
	tk=sc=inlen=num=0;
	memset(buf,'\0',strlen(buf));
	memset(info,'\0',strlen(info));

	memset(reader,'\0',strlen(reader));
	nread=read(client_sockfd,buf,SIZE);
    if(nread==0){
    	printf("\nClosing Client\n");
    	break;
    }

    buf[nread]='\0';
    printf("\nRequest\t: %s\n",buf);
    sscanf(buf,"%c %d %d %d %n",&cmd,&tk,&sc,&inlen,&num);
    cmd=toupper(cmd);


    switch(cmd)
    {
    case 'I' : // sends the information about tracks and sector
    	sprintf(info,"%d %d",track,sec);
    	len = strlen(info);
    	info[len]='\0';
    	printf("Info : %s\n",info);
    	write(client_sockfd, info, strlen(info));
    	break;
    	/*
    	 * Here when the r command is received, it calculates the block to read from maps the file and reads the file.
    	 * If there is any error it exits sending the client an error message
    	 */
    case 'R':
    	if(tk>0&&sc>0){

				readfrm = (tk - 1) * sec + (sc - 1);
				printf("Read from Block : %jd\n", (intmax_t) readfrm);
				sprintf(info, "%jd", (intmax_t) readfrm);

				blkmap = (struct block *) mmap(0, fsize,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if (blkmap == MAP_FAILED) {
					close(fd);
					perror("Error mmapping the file");
					exit(EXIT_FAILURE);
				}
				memcpy(reader+1,blkmap[readfrm].val,BS);

				if (readfrm < totblk) {
					reader[0] = '1';
				}
				else
				{
					reader[0]='0';
				}
				if (munmap(blkmap, fsize) == -1) {
					perror("Error un-mmapping the file");
					// decide here whether to close(fd) and exit() or not. Depends...
				}
				write(client_sockfd, reader, BS+1);
    	}
    	else {
					bin = 0;
					sprintf(info, "%d : Incorrect Format : r <track> <sector>", bin);
					write(client_sockfd, info, strlen(info));
				}
				break;
/*
 * Here when the w command is recieved, it calculates the block to write to maps the file and writes to the file.
 * If there is any error it exits sending the client an error message
 */
    case 'W': // write case file
    			if(tk>0&&tk<=track&&sc>0&&sc<=sec)
    			{
    			writeto = (tk - 1) * sec + (sc - 1);
				printf("Write in Block : %jd\n", (intmax_t) writeto);
				printf("\nThe buffer starts at : %d\n", num);
				blkmap = (struct block *) mmap(0, fsize,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if (blkmap == MAP_FAILED) {
					close(fd);
					perror("Error mmapping the file");
					exit(EXIT_FAILURE);
				}
				memset(blkmap[writeto].val,'\0',BS);
				memcpy(blkmap[writeto].val, buf+num, inlen);
				bin=1;
				sprintf(info,"%d", bin);
				write(client_sockfd, info, strlen(info));
				if (munmap(blkmap, fsize) == -1) {
				    	perror("Error un-mmapping the file");
				    	// decide here whether to close(fd) and exit() or not.
				        }
    			}
    			else
    			{
    				bin = 0;
					sprintf(info,"%d : Incorrect Format : w <track#(1-%d)> <Sec#(1-%d)> <len> <data>", bin,track,sec);
					write(client_sockfd, info, strlen(info));
				}
				break;

    default :
    	write(client_sockfd, cmdError, strlen(cmdError));
    }
 }
    close(client_sockfd);
  }
  free(cmdError);
  free(info);
}

