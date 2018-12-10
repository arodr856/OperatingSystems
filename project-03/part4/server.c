#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 1024
#define FS 100
#define F 0
#define D 1
#define BS 128
char buf[SIZE],fbuf[SIZE];
int blks,i;
char tempbuf[SIZE];
char ftemp[SIZE];
char tem[SIZE];
char pFile[SIZE];
char pFAT[SIZE];
char tempfTab[50];
char *fblk;
char wrbuf[BS+1];
int currdir=-1;
char *pwd[20];
int val=1;




/*===================================================================================*/
	//Global Variables for fserver to connect to dserver as a client
 int sockfd;
 int nread=0;
 int track,sec;

 /*===================================================================================*/
 //Global Variables for fserver to connect with fclient

 	int fsockfd, fclient_sockfd;
	int fnread=0;


/*
 * Structure for File Table
 */
typedef struct{
	char *fname;
	int fsize;
	int stblk;
	int dir;
	int pDir;
}fileTable;

fileTable *fTab;
/*
 * Structure for FAT Table
 */
typedef struct{
	int fb;
	int nxt;
}fileFat;

fileFat *fFat;
//Resets the entire FAT Table
void resetfFat()
{
	for(i=0;i<20;i++){
		pwd[i]=(char *)malloc(sizeof(char)*10);
		memset(pwd[i],'\0',10);
	}
	strcpy(pwd[0],"root/");
	int i;
	for(i=0;i<blks;i++){
		fblk[i]=0;
		fFat[i].fb=0;
		fFat[i].nxt=0;
	}
	printf("FAT Table reset Success\n");
}
//Print the entire FAT table. Created for self reference
void printFAT(){
	printf("FB\t|\tNxt\n");
	for(i=0;i<blks;i++){
		printf("%d\t|\t%d\n",fFat[i].fb,fFat[i].nxt);
	}
}

//Print the entire File table. Created for self reference
void printTAB(){
	printf("Name\t| Size\t| Blk\t| Dir\t| ParentDir\n");
	for(i=0;i<FS;i++){
		printf("%s\t| %d\t| %d\t| %d\t| %d\n",fTab[i].fname,fTab[i].fsize,fTab[i].stblk,fTab[i].dir,fTab[i].pDir);
	}
}


//Resets the entire File Table
void resetfTab(){
	int i;
	for(i=0;i<FS;i++)
	{
		fTab[i].fname=(char *)malloc(sizeof(char)*9);
		strcpy(fTab[i].fname,"\0");
		fTab[i].stblk=0;
		fTab[i].dir=F;
		fTab[i].fsize=0;
		fTab[i].pDir=-1;
		currdir=-1;

	}
	printf("File Table reset Success\n");
}

//Print the file detail for a given index. Created for self reference

void fDetail(int v)
{
	printf("File Name : %s\n",fTab[v].fname);
	printf("Parent Directory : %d\n",fTab[v].pDir);
	printf("Is Dir : %d\n",fTab[v].dir);
	printf("Start Block : %d\n",fTab[v].stblk);
	printf("File Size : %d\n",fTab[v].fsize);

}

//Get the free FAT table block to add the next entry
int freeBlk(){
	printf("freeBlk : \n");
	int i;
	for(i=2;i<blks;i++){
		if(fFat[i].fb==0)
			return i;
	}
	return 0;
}

//Get the free file table block to add the next entry
int freeTablk(){
	printf("freeTablk : \n");
	int i;
	for(i=0;i<FS;i++){
		if(fTab[i].stblk==0)
			return i;
	}
	return 0;
}

//Reset the file block for for the passed filename to be over written
int fBlkreset(char *name){
	printf("fBlkreset : \n");
	for(i=0;i<FS;i++)
	{
		if(strcmp(name,fTab[i].fname)==0)
		{
			int st=0,temp=0;
			st=fTab[i].stblk;
			while(fFat[st].nxt!=1){
				temp=fFat[st].nxt;
				fFat[st].nxt=0;
				st=temp;
				fFat[st].fb=0;
			}
			return 1;
		}
	}
	return 0;
}

//Get the Free block count from the Fat Table and return the count

int freeBlkCnt(){
	printf("freeBlkCnt : \n");
	int cnt=0;
	for(i=2;i<blks;i++)
	{
		if(fFat[i].fb==0)
		cnt++;
	}
	return cnt;
}

// Gets the data and calculates the needed blocks
int Blkcnt(int data)
{
	printf("Blkcnt : \n\n");
	int bcnt=0;
	bcnt = data/BS;
	if(data%BS!=0)
	{
		bcnt++;
	}
	return bcnt;
}
//Check if the file exists
int fileExists(char *name)
{
	printf("fileExists : \n");
	for(i=0;i<FS;i++){
			if(strcmp(name,fTab[i].fname)==0&&currdir==fTab[i].pDir){
				if(fTab[i].dir==0){
				return i;
				}else{
					continue;
				}
			}
		}
	return (-1);
}
//Check whther the directory exists
int dirExists(char *name)
{
	printf("DirExists : \n");
	for(i=0;i<FS;i++){
		if(strcmp(name,fTab[i].fname)==0&&currdir==fTab[i].pDir){
			if(fTab[i].dir==1){
			return i;
			}else{
				continue;
			}
		}
	}
	return (-1);
}

//Function to check whther the directory is empty
int isEmpty(int ind){
	for(i=0;i<FS;i++){
		if(fTab[i].pDir==ind){
			return 0;
		}
	}
	return 1;
}
//Gets block nmber and sends the Track Number
int tget(int b)
{
	int tr=(b/sec)+1;
	return tr;
}

//Gets block nmber and sends the sector Number
int sget(int b){
	int sc=(b%sec)+1;
		return sc;
}

/**************************************************************************************************************************************************
   *
   * Code for to format the file system and the remaining file functions
   *
   **************************************************************************************************************************************************/
/*
 * This function initializes all the values and formats the disk
 */
int fFormat(){
	resetfFat();
	resetfTab();
	return 0;
}

/*
 *This function Creates a file for the given file name by choosing a free block
 * based on the existence of the file.It takes the input parameter as filename and returns Error or success
 */


int fCreate(char *name)
{
	int i;
	int frblk=freeBlk();
	//printf("freeblk in fcreate : %d\n\n",frblk);
	int frtab=freeTablk();
	// printf("File Name in fcreate: %s\n\n ",name);

		if(fileExists(name)!=-1)
		{
			return (1);
		}
		if(frblk==0)
		{
			return (2);
		}
		if(fTab[frtab].stblk==0)
		{
			strcpy(fTab[frtab].fname,name);
			fTab[frtab].fsize=0;
			fTab[frtab].stblk=frblk;
			fFat[frblk].fb=frblk;
			fFat[frblk].nxt=1;
			fTab[frtab].dir=F;
			fTab[frtab].pDir=currdir;
			fDetail(frtab);

			return (0);
		}

	return (3);
}

/*
 *This function deletes the given file name from the start block
 * based on the existence of the file is empty.It takes the input parameter as filename
 * and returns Error or success
 */


int fDelete(char *name)
{
	printf("File Name : %s\n", name);
	nread = 0;
	int fInd = 0;
	fInd = fileExists(name);
	printf("File Index : %d\n", fInd);
	int stblk;
	int nxtblk;
	if(fInd==-1)
		{
			return 1;
		}
	if(fBlkreset(name)!=1)
		{
			return 3;
		}
	stblk=fTab[fInd].stblk;
	printf("stblk = %d\n",stblk);
	fFat[stblk].fb=0;
	fFat[stblk].nxt=0;
	memset(fTab[fInd].fname,'\0',9);
	//memset(fTab[fInd].pDir,'\0',9);
	fTab[fInd].stblk=0;
	fTab[fInd].dir=F;
	fTab[fInd].pDir=-1;
	fTab[fInd].fsize=0;
	printf("After Delete : \n");
	fDetail(fInd);
	printFAT();
	return 0;
}
/*
 *This function Lists the files based on the parameter 0 or 1. it takes the input parameter as 0 or 1
 */
int fList(int p)
{
	char *fDet;
	fDet=(char *)malloc(sizeof(char)*20);
	memset(tempbuf,'\0',SIZE);
	//wfTab();
	if(p==0)
	{
		for(i=0;i<FS;i++){
			if(strcmp(fTab[i].fname,"\0")!=0&&currdir==fTab[i].pDir){
			sprintf(fDet,"%s%s",fTab[i].fname,"\n");
			strcat(tempbuf,fDet);
			memset(fDet,'\0',20);
			}else if (i == 14&&(strlen(tempbuf)==0)) {
				return 1;
			}
		}
		tempbuf[strlen(tempbuf)]='\0';
		return 0;
	}
	if(p==1)
		{
			for(i=0;i<FS;i++){
				if(strcmp(fTab[i].fname,"\0")!=0&&currdir==fTab[i].pDir){
				sprintf(fDet,"%s %d%s",fTab[i].fname,fTab[i].fsize,"\n");
				strcat(tempbuf,fDet);
				memset(fDet,'\0',20);
				}
				else if (i == 14&&(strlen(tempbuf)==0)) {
				return 1;
			}
			}
			tempbuf[strlen(tempbuf)]='\0';
			return 0;
		}
	return 2;
}
/*
 *This function Reads the given file name from the startblock
 * based on the existence of the file.It takes the input parameter as filename and returns Error or success
 */

int fRead(char *name)
{
	printf("File Name : %s\n",name);
	nread=0;
	int fInd=0;
	int blkcnt=0;
	fInd=fileExists(name);
	printf("File Index : %d\n",fInd);
	int stblk;
	int nxtblk;
	int len=0;
	int totlen=0;
	int t=0,s=0;
	int dlen=0;
	int temp;
	memset(tempbuf,'\0',SIZE);

	memset(tem,'\0',SIZE);
	if(fInd==-1)
		{
			return 1;
		}

	stblk=fTab[fInd].stblk;
	if(fTab[fInd].fsize==0)
	{
		return 2;
	}

	while (1)
	{
		t=tget(stblk);
		s=sget(stblk);
		sprintf(ftemp,"%s %d %d","R",t,s);
		nread = write(sockfd, ftemp, strlen(ftemp));
		memset(ftemp,'\0',SIZE);
		nread = read(sockfd, buf, SIZE);
		strcat(tem,buf+1);
		//memcpy(ftemp+totlen,buf+1,nread-1);
		printf("tem : %s\n",tem);
		totlen=strlen(tem);
		printf("nread : %d totlen :%d\n",nread,totlen);
		if(fFat[stblk].nxt==1)
		{
			break;
		}
		stblk=fFat[stblk].nxt;
	}
	ftemp[totlen]='\0';
	sprintf(tempbuf,"%s %d %s","0:",totlen,tem);
	//fnread = write(fclient_sockfd, tempbuf, strlen(tempbuf));
	return 0;
}
/*
 *This function Writes the given file name from the startblock
 * based on the existence of the file.It takes the input parameter as filename and returns Error or success
 * and stores it in a global buffer
 */

int fWrite(char *name,char *data,int len)
{
	nread=0;
	int blkcnt=0;
	int freebcnt=0;
	int fInd=0;
	fInd=fileExists(name);
	int stblk;
	int nxtblk;
	int dlen=0;
	int t=0,s=0;
	int nf=0;
	int sfb;
	printf("fWrite data : %s\n",data);

	if(fInd==-1)
	{
		return 1;
	}
	freebcnt=freeBlkCnt();
	blkcnt=Blkcnt(len);
	if(freebcnt<blkcnt){
		return 2;
	}
	if(fBlkreset(name)!=1)
	{
		return 3;
	}
	sfb=stblk=fTab[fInd].stblk;
	printf("Start blk : %d\n",stblk);
	fTab[fInd].fsize=len;
	//printf("blkcnt = %d\n",blkcnt);
	for(i=1;i<=blkcnt;i++)
	{
		fFat[stblk].fb=sfb;
		memset(wrbuf,'\0',BS);
		dlen=(i-1)*BS;
		memcpy(wrbuf,data+dlen,BS);
		t=tget(stblk);
		s=sget(stblk);
		sprintf(ftemp,"%s %d %d %ld %s","W",t,s,strlen(wrbuf),wrbuf);
		printf("Request to send in fWrite: %s\n",ftemp);
		nread=write(sockfd,ftemp,strlen(ftemp));
		nread = read(sockfd, buf,SIZE);
		if (i == blkcnt) {
			fFat[stblk].fb=stblk;
			fFat[stblk].nxt = 1;
		} else {
			nf = freeBlk();
			fFat[stblk].fb=stblk;
			fFat[stblk].nxt = nf;
			stblk=nf;
			printf("fFat[stblk].fb : %d,nxt : %d,stblk: %d\n",fFat[stblk].fb,fFat[stblk].nxt,stblk);
		}
	}
	fDetail(fInd);
	return 0;
}
/**************************************************************************************************************************************************
   *
   * Code for to perform various directory functions
   *
   **************************************************************************************************************************************************/

/*
 *This function Creates a directory for the given dir name by choosing a freeblock
 * based on the existence of the directory.It takes the input parameter as dirname and returns Error or success
 */

int dCreate(char *name)
{
	int i;
	int frblk=freeBlk();
	int frtab=freeTablk();
	int fInd=dirExists(name);

		if(fInd!=-1&&(fTab[fInd].dir==1))
		{
			return (1);
		}
		if(frblk==0)
		{
			return (2);
		}
		if(fTab[frtab].stblk==0)
		{
			strcpy(fTab[frtab].fname,name);
			fTab[frtab].fsize=0;
			fTab[frtab].stblk=frblk;
			fFat[frblk].fb=frblk;
			fFat[frblk].nxt=1;
			fTab[frtab].dir=D;
			fTab[frtab].pDir=currdir;
			fDetail(frtab);
			printTAB();
			return (0);
		}
	return (3);
}

/*
 * This functions takes the dir name as input and changes the current working directory to the given name if the directory exists
 * It returns a success or error based on the input
 */

int changeDir(char* name){

	int dInd=dirExists(name);
	printTAB();
	printf("Curr Dir : %d\n",currdir);
	if(strcmp(name,"..")==0)
	{
		if(currdir==-1){
			return 2;
		}
		else{
				if(val!=1){
					val--;
					memset(pwd[val],'\0',10);
				}
				currdir=fTab[currdir].pDir;
				printf("New Curr Dir : %d\n",currdir);
		}
	}else if (dInd==-1){
		return 1;
	}else{
		strcpy(pwd[val],name);
		strcat(pwd[val],"/");
		val++;
		printf("dInd : %d",dInd);
		currdir=dInd;
		return 0;
	}
	return 0;
}

/*
 *This function deletes the given directory name from the startblock
 * based on the existence of the directory and if the directory is empty.It takes the input parameter as dirname
 * and returns Error or success
 */


int dDelete(char *name)
{
	printf("Dir Name : %s\n", name);
	nread = 0;
	int fInd = 0;
	fInd = dirExists(name);
	printf("Dir Index : %d\n", fInd);
	int stblk;
	int nxtblk;
	int res;
	res=isEmpty(fInd);
	if(fInd==-1)
		{
			return 1;
		}
	if(res==0)
		{
			return 3;
		}

	stblk=fTab[fInd].stblk;
	printf("stblk = %d\n",stblk);
	fFat[stblk].fb=0;
	fFat[stblk].nxt=0;
	memset(fTab[fInd].fname,'\0',9);
	fTab[fInd].stblk=0;
	fTab[fInd].dir=F;
	fTab[fInd].pDir=-1;
	fTab[fInd].fsize=0;
	printf("After Delete : \n");
	fDetail(fInd);
	printFAT();
	printTAB();
	return 0;
}


int main(int argc,char *argv[])
{
   /*===================================================================================*/
	//Variables for fserver to connect to dserver as a client
  struct sockaddr_in serv_addr;
  struct hostent *host;
  uint16_t dserv_port,fserv_port;
  char *ip;

  /*===================================================================================*/
  //Variables for fserver to connect with fclient

    socklen_t flen;
	struct sockaddr_in fserv_addr, fclient_addr;


   /*===================================================================================*/
   //Variables for fserver to dserver

   char ts[SIZE];



   fTab=(fileTable *)malloc(sizeof(fileTable)*FS);
   int fTabSize=sizeof(fileTable)*FS;
   printf("SIZE OF FILE TABLE : %ld\n\n\n",sizeof(fileTable)*FS);
   resetfTab();




   /*===================================================================================*/
   //Variables for fserver to fclient

      char *cmd,*fname;
      int inlen,num,cmdID=0;

      cmd=(char *)malloc(sizeof(char)*10);
      fname = (char *)malloc(sizeof(char)*10);
      char ch;
      int cval,wval,rval,fval,lval,dval,param;//C switch File variable
      int mkval,rmval,pwval,cdval;
      char *info;
      info =(char *)malloc(sizeof(char)*SIZE);





   /*===================================================================================*/

   //Checking for Command line Arguments
   if (argc != 4)
  {
    fprintf(stderr,"\nUsage: /.fserver <Fserver PORT> <IP> <PORT>\n\n");
    exit(1);
  }
  fserv_port=atoi(argv[1]);
  ip=argv[2];
  dserv_port=atoi(argv[3]);

   printf("\n Fserver Port\t: %d",fserv_port);
   printf("\n Dserver IP\t: %s",ip);
   printf("\n Dserver Port\t: %d\n\n",dserv_port);


  /**************************************************************************************************************************************************
   *
   * Code for fserver as a dclient to dserver
   *
   **************************************************************************************************************************************************/
  /* create End point */
  if ((sockfd = socket(AF_INET,SOCK_STREAM, 0))< 0) {
    perror(NULL);
    exit(2);
  }

  /* connect to server */
  serv_addr.sin_family = AF_INET;
  host = gethostbyname(argv[2]);
  memcpy(&serv_addr.sin_addr.s_addr, host->h_addr, host->h_length);
  serv_addr.sin_port = htons(dserv_port);
  printf("\nSending Connection request to Dserver...\n");
  if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr))< 0) {
    perror(NULL);
    exit(3);
  }
  printf("\nSuccessfully Connected to Dserver..\n");

  //Getting the number of tracks and sectors from dserver
          nread=write(sockfd,"i",1);
       	  nread = read(sockfd, buf,SIZE);
       	  printf("\nDServer\t: %s\n",buf);
       	  sscanf(buf,"%d %d",&track,&sec);
       	  printf("Track\t: %d\n",track);
       	  printf("Sectors\t: %d\n",sec);
       	  blks=track*sec;
       	  printf("Blocks : %d\n\n",blks);
       	  fblk=(char *)malloc(sizeof(char)*blks);
       	  fFat=(fileFat *)malloc(sizeof(fileFat)*blks);

       	  //Resetting the file Fat table to 0's;
       	  resetfFat();




  /**************************************************************************************************************************************************
     *
     * Code for fserver as a server to fclient
     *
     ************************************************************************************************************************************************/

  /* create endpoint for fserver,server for fclient */
   if ((fsockfd = socket(AF_INET,SOCK_STREAM, 0))< 0)
   {
     perror(NULL);
     exit(2);
   }

   /* bind address fserver for fclient */
   fserv_addr.sin_family = AF_INET;
   fserv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   fserv_addr.sin_port = htons(fserv_port);

   if (bind(fsockfd, (struct sockaddr*)&fserv_addr, sizeof(fserv_addr)) < 0)
   {
     perror(NULL);
     exit(3);
   }
   /**************************************************************************************************************************************************
      *
      * Code for listening from fserver socket and accepting fclient.
      * fclient commands will be read and processed here.
      * Output to fclient socket is written here
      *
      ************************************************************************************************************************************************/
   /* specify queue */
   listen(fsockfd, 5);
   for (;;) {
	 printf("\nListening socket for Incoming Connections ..\n");
     flen = sizeof(fclient_addr);
     fclient_sockfd = accept(fsockfd,(struct sockaddr*)&fclient_addr,&flen);
     printf("\nAccepting Connection Successful..\n");
     if (fclient_sockfd == -1) {
       perror(NULL);
       continue;
     }
     printf("\nFserver waiting to read from new Connection Fclient...\n");



     	 while(1){

     	      	memset(fbuf,'\0',strlen(fbuf));
     	      	memset(buf,'\0',strlen(buf));
     	      	memset(cmd,'\0',10);
     	      	memset(info,'\0',SIZE);
     	      	memset(tempbuf,'\0',SIZE);
     	      	memset(fname,'\0',10);
     	      	inlen=num=-1;


     	      	fnread=read(fclient_sockfd,fbuf,SIZE);
     	      	if(fnread==0){
     	          	printf("\nClosing Client\n");
     	          	break;
     	          }

     	      		printf("\nRequest from fclient\t: %s\n",fbuf);
     	      	    sscanf(fbuf,"%s %s %d %n",cmd,fname,&inlen,&num);
     	      	    /*
     	      	     * Here If else statements are used to compare the given cmds
     	      	     * and assign it a proper cmdID to use it in a switch case.
     	      	     * The values are assigned from 1 - 10 for various commands
     	      	     */
     	      	    if(strcasecmp(cmd,"mkdir")==0)
     	      	    {
     	      	    	cmdID=7;
     	      	    }else if(strcasecmp(cmd,"cd")==0)
     	      	    {
     	      	    	cmdID=8;
     	      	    }else if(strcasecmp(cmd,"pwd")==0)
     	      	    {
     	      	    	cmdID=9;
     	      	    }else if(strcasecmp(cmd,"rmdir")==0)
     	      	    {
     	      	    	cmdID=10;
     	      	    }else if(strcasecmp(cmd,"help")==0){
     	      	    	cmdID=11;
     	      	    }
     	      	    else if(strlen(cmd)==1)
     	      	    {
     	      	    	ch=cmd[0];
     	      	    	ch=toupper(ch);
     	      	    	if(ch=='F')
     	      	    		cmdID=1;
     	      	    	else if(ch=='C')
     	      	    		cmdID=2;
     	      	    	else if(ch=='D')
     	      	    	    cmdID=3;
     	      	    	else if(ch=='L')
     	      	    	     cmdID=4;
     	      	    	else if(ch=='R')
     	      	    	     cmdID=5;
     	      	    	else if(ch=='W')
     	      	    	     cmdID=6;
     	      	    	else
     	      	    		cmdID=0;
     	      	    		ch='\0';
     	      	    }else{
     	      	    	cmdID=0;
     	      	    }

     	      	    /*
     	      	     * Switch case based on the above cmdID to perform the various functions on the given command
     	      	     */

     	      	    printf("File Name : %s\n\n ",fname);

     	      	    switch(cmdID)
     	      	    {
     	      	    case 1: //File Format Case
     	      	    	printf("\nRequest Cmd and CmdID : %c %d\n",ch,cmdID);
     	      	    	fval=1;
     	      	    	fval=fFormat();
     	      	    	if (fval == 0) {
     	      	    		sprintf(info, "%s", "0 : Format Successful");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	} else if (fval == 1) {
     	      	    		sprintf(info, "%s", "1 : Format Unsuccessful");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	}
     	      	    	break;
     	      	    case 2://File Create Case
     	      	    	printf("\nRequest Cmd and CmdID : %c %d\n",ch,cmdID);
     	      	    	if ((strcmp(fname, "\0") == 0)) {
     	      	    		sprintf(info, "%s","Error : Create Format : c <filename>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}
     	      	    	cval = fCreate(fname);
     	      	    	if(cval==0)
     	      	    	{
     	      	    		sprintf(info,"%s","0 : File Created Successfully");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(cval==1)
     	      	    	{
     	      	    		sprintf(info,"%s","1 : File Name Already Exists");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(cval==2)
     	      	    	{
     	      	    		sprintf(info,"%s","2 : Disk Full. Delete Some data");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else{
     	      	    		sprintf(info,"%s","2 : File Creation Unsuccessful");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	break;
     	      	    case 3: //File Delete case
     	      	    	printf("\nRequest Cmd and CmdID : %c %d\n",ch,cmdID);
     	      	    	if ((strcmp(fname, "\0") == 0)) {
     	      	    		sprintf(info, "%s","Error : Delete Format : d <filename>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}
     	      	    	dval = fDelete(fname);
     	      	    	if(dval==0)
     	      	    	{
     	      	    		sprintf(info,"%s","0 : File Deleted Successfully");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(dval==1)
     	      	    	{
     	      	    		sprintf(info,"%s","1 : File Name Doesn't Exists");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(dval==3)
     	      	    	{
     	      	    		sprintf(info,"%s","2 : File Reset Block Unsuccessful");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else{
     	      	    		sprintf(info,"%s","2 : File Deletion Unsuccessful");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	break;
     	      	    case 4: //File List Case
     	      	    	printf("\nRequest Cmd and CmdID : %c %d\n",ch,cmdID);
     	      	    	if ((strcmp(fname, "\0") == 0)) {
     	      	    		sprintf(info, "%s","Error : List Format : l <0 or 1>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}
     	      	    	if(strcmp(fname,"0")==0){
     	      	    		param=0;
     	      	    		lval=fList(param);
     	      	    	}
     	      	    	else if(strcmp(fname,"1")==0){
     	      	    		param=1;
     	      	    		lval=fList(param);
     	      	    	}
     	      	    	else{
     	      	    		sprintf(info, "%s", "Error : List Format : l <0 or 1>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}

     	      	    	if (lval == 0) {
     	      	    		sprintf(info, "%s", "0 : File Listed Successfully");
     	      	    		fnread = write(fclient_sockfd, tempbuf, strlen(tempbuf));
     	      	    	} else if (lval == 1) {
     	      	    		sprintf(info, "%s", "1 : No files to display");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	} else if (lval == 2) {
     	      	    		sprintf(info, "%s", "2 : Error Listing Files");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	}
     	      	    	break;
     	      	    case 5: //File Read Case
     	      	    	printf("\nRequest Cmd and CmdID : %c %d\n",ch,cmdID);
     	      	    	if ((strcmp(fname, "\0") == 0)) {
     	      	    		sprintf(info, "%s","Error : Read Format : r <filename>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}
     	      	    	rval = fRead(fname);

     	      	    	if (rval == 0) {
     	      	    		sprintf(info, "%s", "0 : File Read Successfully");
     	      	    		fnread = write(fclient_sockfd, tempbuf, strlen(tempbuf));
     	      	    	} else if (rval == 1) {
     	      	    		sprintf(info, "%s", "1 : File Name Doesn't Exists");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	} else if (rval == 2) {
     	      	    		sprintf(info, "%s", "2 : File Empty. No data");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	} else {
     	      	    		sprintf(info, "%s", "2 : File Creation Unsuccessful");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	}

     	      	    	break;
     	      	    case 6: //File Write case
     	      	    	printf("\nRequest Cmd and CmdID : %c %d\n",ch,cmdID);

     	      	    	if((strcmp(fname,"\0")==0)||inlen==-1||num==-1)
     	      	    	{
     	      	    		sprintf(info, "%s", "Error : Write Format : w <filename> <length> <data>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}

     	      	    	memcpy(tempbuf,fbuf+num,inlen);
     	      	    	printf("Data : %s\n\n",tempbuf);
     	      	    	wval=fWrite(fname,tempbuf,inlen);
     	      	    	if (wval == 0) {
     	      	    		sprintf(info, "%s", "0 : File Written Successfully");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	} else if (wval == 1) {
     	      	    		sprintf(info, "%s", "1 : File Name doesn't Exists");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	} else if (wval == 2) {
     	      	    		sprintf(info, "%s", "2 : Blocks Full. Delete Some data");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	} else {
     	      	    		sprintf(info, "%s", "2 : File Writing Unsuccessful");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	}
     	      	    	break;
     	      	    case 7: //Makedir Case
     	      	    	printf("\nRequest Cmd and CmdID : %s %d\n",cmd,cmdID);
     	      	    	if ((strcmp(fname, "\0") == 0)) {
     	      	    		sprintf(info, "%s","Error : MakeDir Format : mkdir <filename>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}
     	      	    	mkval = dCreate(fname);
     	      	    	if(mkval==0)
     	      	    	{
     	      	    		sprintf(info,"%s","0 : Directory Created Successfully");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(mkval==1)
     	      	    	{
     	      	    		sprintf(info,"%s","1 : Directory Name Already Exists");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(mkval==2)
     	      	    	{
     	      	    		sprintf(info,"%s","2 : Disk Full. Delete Some data");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else{
     	      	    		sprintf(info,"%s","2 : Directory Creation Unsuccessful");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	break;
     	      	    case 8: //Change Directory Case
     	      	    	printf("\nRequest Cmd and CmdID : %s %d\n",cmd,cmdID);
     	      	    	if ((strcmp(fname, "\0") == 0)) {
     	      	    		sprintf(info, "%s","Error : ChangeDir Format : cd <filename or ..>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}
     	      	    	cdval=changeDir(fname);
     	      	    	if(cdval==0)
     	      	    	{
     	      	    		sprintf(info,"%s","0 : Directory Changed Successfully");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(cdval==1)
     	      	    	{
     	      	    		sprintf(info,"%s","1 : Directory Name Doesn't Exists");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(cdval==2)
     	      	    	{
     	      	    		sprintf(info,"%s","2 : You are currently in the root Directory");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else{
     	      	    		sprintf(info,"%s","2 : Directory Change Unsuccessful");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	break;
     	      	    case 9: //Present Working Directory Case
     	      	    	printf("\nRequest Cmd and CmdID : %s %d\n",cmd,cmdID);
     	      	    	memset(ftemp,'\0',SIZE);
     	      	    	strcat(ftemp,pwd[0]);
     	      	    	for(i=1;i<20;i++){
     	      	    		strcat(ftemp,pwd[i]);
     	      	    	}
     	      	    	fnread = write(fclient_sockfd, ftemp, strlen(ftemp));
     	      	    	break;
     	      	    case 10: // Remove Directory Case
     	      	    	printf("\nRequest Cmd and CmdID : %s %d\n",cmd,cmdID);
     	      	    	if ((strcmp(fname, "\0") == 0)) {
     	      	    		sprintf(info, "%s","Error : Remove Directory Format : rmdir <filename>");
     	      	    		fnread = write(fclient_sockfd, info, strlen(info));
     	      	    		break;
     	      	    	}
     	      	    	rmval = dDelete(fname);
     	      	    	if(rmval==0)
     	      	    	{
     	      	    		sprintf(info,"%s","0 : Directory Deleted Successfully");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(rmval==1)
     	      	    	{
     	      	    		sprintf(info,"%s","1 : Directory Name Doesn't Exists");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else if(rmval==3)
     	      	    	{
     	      	    		sprintf(info,"%s","2 : Directory is not Empty. Please delete the content before rmdir");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	else{
     	      	    		sprintf(info,"%s","2 : Directory Deletion Unsuccessful");
     	      	    		fnread=write(fclient_sockfd,info,strlen(info));
     	      	    	}
     	      	    	break;
     	      	    case 11: //Help Case
     	      	    	printf("\nRequest Cmd and CmdID : %s %d\n",cmd,cmdID);
     	      	    	memset(tem,'\0',SIZE);
     	      	    	memset(ftemp,'\0',SIZE);
     	      	    	memset(info,'\0',SIZE);
     	      	    	sprintf(tem,"%s","\nHelp :\n\t1.  Create File\t: c <FileName>\n\t2.  Write File\t: w <FileName> <Len> <data>\n\t3.  Read File\t: r <FileName>\n\t4.  Delete File\t: d <FileName>\n\t5.  Format\t: f\n\t6.  List Files\t: l <0 or 1>\n\t");
     	      	    	sprintf(ftemp,"%s","7.  MakeDir\t: mkdir <DirName>\n\t8.  RemoveDir\t: rmdir <DirName>\n\t9.  Change Dir\t: cd <filename or ..>\n\t10. Present Dir\t: pwd\n\t11. Help\t: help\n\t12. Exit\t: exit");
     	      	    	strcat(info,tem);
     	      	    	strcat(info,ftemp);
     	      	    	fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	break;
     	      	    default:
     	      	    	sprintf(info,"%s","InCorrect Command : Please enter 'help' to get list of commands");
     	      	    	fnread = write(fclient_sockfd, info, strlen(info));
     	      	    	break;
     	      	    }
     	      }

     close(fclient_sockfd);
   }
   close(sockfd);
   exit(0);
   return 0;
}

