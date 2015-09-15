/*
** server.c -- a stream socket server
*
*  Author : Achut Nandam
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
 
/* Not technically required, but needed on some UNIX distributions */
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>


#define PORT "3490"		// the port users will be connecting to
#define BACKLOG 10		// how many pending connections queue will hold
#define PERMISSION "/home/curiosity/Documents/cns/permission.sh"
#define BUFFERT 512
	/*********** variables begin  ***********/

int sockfd;			// listen on sock_fd, new connection on new_fd
struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;	// connector's address information
socklen_t sin_size;
struct sigaction sa;
int yes = 1;
char s[INET6_ADDRSTRLEN];
int rv;
char buffer[1000001];
char buf[BUFFERT];
char loc[1024][1024];
int num = 0;
	/********** variables end ***********/


struct node
{
    struct node *prev;
    char *file_name;
    char *work;
    struct node *next;
}*h,*temp,*temp1,*temp2,*temp4;


void create(char *file_name , char *work)
{
    temp =(struct node *)malloc(1*sizeof(struct node));
    temp->prev = NULL;
    temp->next = NULL;
    temp->file_name = file_name;
    temp->work = work;
}

void insert(char *file_name , char *work)
{
    if (h == NULL)
    {
        create(file_name,work);
        h = temp;
        temp1 = h;
    }
    else
    {
        create(file_name,work);
        temp1->next = temp;
        temp->prev = temp1;
        temp1 = temp;
    }
}

void delete(char *file_name , char *work)
{
	temp2 = h;
	while(temp2 != NULL){
		if(!strcmp(temp2->file_name ,file_name) && !strcmp(temp2->work , work)){
			if(temp2->prev == NULL)
				h = temp2->next;
			else if(temp2->next == NULL){
				temp2->prev->next = NULL;
			}			
			else{
				temp2->prev->next = temp2->next;
				temp2->next->prev = temp2->prev;
			} 
			printf("Done deleting\n");
			return ;
		}
	}
	printf("Failed delete\n");
}

int search(char *file_name , char *work)
{
	temp2 = h;
	while(temp2 != NULL){
		if(!strcmp(temp2->file_name ,file_name) && !strcmp(temp2->work , work)){
			return 1;		
		}
	}
	return 0;
}

typedef struct variable
{

  int socketfd;

} banana;

void
sigchld_handler (int s)
{
  while (waitpid (-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *
get_in_addr (struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
    {
      return &(((struct sockaddr_in *) sa)->sin_addr);
    }
  return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}


int
sendall (int s, char *buf, int len)
{
  int total = 0;
// how many bytes we've sent
  int bytesleft = len;		// how many we have left to send
  int n;
  while (total < len && bytesleft > 0)
    {
      n = send (s, buf + total, bytesleft, 0);
      if (n == -1)
	{
	  break;
	}
      total += n;
      bytesleft -= n;
    }
  // *len = total;                       // return number actually sent here
  return n == -1 ? -1 : 0;	// return -1 on failure, 0 on success
}

void listdir(const char *name, int level ,char folder[1024][1024] ,int *p,int new_fd)
{
	//write(new_fd,name,sizeof name);
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;
    if (!(entry = readdir(dir)))
        return;
	//char folder[1024][1024];
	//int p = 0;
    do {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            int len = 0;//snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            //path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
		strcpy(path,entry->d_name);
		//char booth[1024];
		len = snprintf(path,sizeof(path)-1,"%*s[%s]$",level*2, "", entry->d_name); 
		path[len] = 0;   
		//printf("%s\n",path);         
		
		int written = write(new_fd,path,strlen(path));		
		if(written <= 0)
			{perror("write");return;}
		//printf("%*s[%s]\n", level*2, "", entry->d_name);
		strcpy(folder[*p], (char *)entry->d_name);
		*p += 1;
		//free(path);
          //  listdir(path, level + 1);
        }
        else
            {
		char path[1024];
		int len = snprintf(path,sizeof(path)-1,"%*s- %s$", level*2, "", entry->d_name);	
		path[len] = 0;
		//printf("%s\n",path);
		int written = write(new_fd,path,strlen(path));
		if(written <= 0)
			{perror("write");return;}	
	    }
    } while (entry = readdir(dir));
    closedir(dir);
    int written = write(new_fd,"DONE$",sizeof("DONE$"));
	if(written <= 0)
			{perror("write");return;}
}

void listthedirupload(int new_fd,char ret[1024])

{

	//int num = 0;
	strcpy(ret,"");
	int ti = 0;
	char cmd[1024];
	int rr;
	char booth[1024];
	printf("num %d\n",num);		
	for(;ti < num;ti++){
		//printf("%s \n",loc[ti]);
		int len = snprintf(booth,sizeof(booth)-1,"   {%s}$",loc[ti]);
		booth[len] = 0;
		int written = write(new_fd,booth,strlen(booth));
		if(written <= 0)
			{perror("write");return;}			
	}
	int size_done = sizeof("DONE$");
	int written = write(new_fd,"DONE$",size_done);
		if(written <= 0)
			{perror("write");return;}
	char path[1024];
	//strcpy(path,cmd);
	while(1){
		//printf("Enter the path\n");
		memset(path,0,sizeof path);
		int bytes = read(new_fd,path,sizeof path);//scan the cmd
		printf("Dire   %s\n",path);
		if(bytes <= 0)
		{
			perror("Error in listthedir");		
			return;
		}
		ti = 0;
		rr = 0;
		for(;ti < num;ti++)
		{
			if(!strcmp(path,loc[ti]))
				{rr = 1;break;}				
		}
		if(rr == 1)
			{written = write(new_fd,"DONE",sizeof("DONE"));
			if(written <= 0)
			{perror("write");return;}
			break;}
		written = write(new_fd,"boo",sizeof("boo"));
		if(written <= 0)
			{perror("write");return;}
	}

/*

	while(1){
		//printf("Enter the path\n");
		int bytes = read(new_fd,cmd,sizeof cmd);//scan the cmd
		if(bytes <= 0)
		{
			perror("Error in listthedir");		
			return ;
		}
		ti = 0;
		rr = 0;
		for(;ti < num;ti++)
		{
			if(!strcmp(cmd,loc[ti]))
				{rr = 1;break;}				
		}
		if(rr == 1)break;
	}
*/	
	read(new_fd,cmd,sizeof cmd);
	char folder[1024][1024];
	int level = 0;
	while(1){
		int p = 0;
		printf("goining to listdir\n");
		printf("path   %s\n",path);
		listdir(path, level ,folder,&p,new_fd);
		printf("Enter the command\n");	
		char cmd[1024];
		memset(cmd,0,sizeof cmd);
		int bytes = read(new_fd,cmd,sizeof cmd);//scan the cmd
		if(bytes <= 0)
		{
			perror("Error in listthedir");		
			return ;
		}
		printf("%s        booooo\n",cmd);
		if(!strcmp(cmd,"back"))
		{
			level = level - 1;
			char c[2];
			c[0] = 65+level;
			c[1] ='\0';
			printf("%s",c);
			bytes = write(new_fd,c,strlen(c));
			if(bytes <= 0 ) 	{perror("here 0");return;}		
			if((bytes = read(new_fd,cmd,sizeof cmd))<= 0) {perror("here 1");return;}
			memset(cmd,0,sizeof cmd);
			if(level < 0)
			{					
				ti = 0;
				for(;ti <num;ti++){
					int len = snprintf(booth,sizeof(booth)-1,"   {%s}$",loc[ti]);
					booth[len] = 0;
					written  = write(new_fd,booth,strlen(booth));if(written <= 0)
			{perror("write");return;}			
				}
				written = write(new_fd,"DONE$",sizeof("DONE$"));if(written <= 0)
			{perror("write");return;}
				//write(new_fd,"-1",sizeof("-1"));
				while(1){
					printf("Enter the path\n");
					memset(path,0,sizeof path);
					int bytes = read(new_fd,path,sizeof path);//scan the cmd
					if(bytes <= 0)
					{
						perror("Error in listthedir");		
						return;
					}
					printf("Dire   %s\n",path);
					ti = 0;
					rr = 0;
					for(;ti < num;ti++)
					{
						if(!strcmp(path,loc[ti]))
							{rr = 1;break;}				
					}
					if(rr == 1)
						{written = write(new_fd,"DONE",sizeof("DONE"));if(written <= 0)
			{perror("write");return;}break;}
					written = write(new_fd,"boo",sizeof("boo"));if(written <= 0)
			{perror("write");return;}
				}
				level += 1;
				read(new_fd,cmd,sizeof(cmd));
			}
			else
			{
				//write(new_fd,"0",sizeof("0"));
				int tp = strlen(path)-1;
				while(tp--)
				{
					if(path[tp] == '/')
						{path[tp]='\0';break;}			
				}
			}
			continue;
		}
		else if(!strcmp(cmd,"folder")){
			memset(cmd,0,sizeof cmd);
			int cc = read(new_fd,cmd,sizeof(cmd));
			if(cc <= 0)
			{
				perror("folder in ");
				return;
		}
			printf("cmdd i fol  %s\n",cmd);
			int flag = 0;
			int i = 0;
			for(i = 0 ; i < p ;i++){
				if(!strcmp(cmd,folder[i])){
					flag = 1;
					break;
				}
			}
			if(flag)
			{
				char pathe[1024];
				printf("DIR BEFORE   %s\n",path);
		    		int len = snprintf(pathe, sizeof(pathe)-1, "%s/%s", path, cmd);
		    		pathe[len] = 0;
				strcpy(path,pathe);
				printf("DIR AFTER   %s\n",path);
				level += 1;
			}
		}
		else if(!strcmp(cmd,"file"))
		{
			memset(cmd,0,sizeof cmd);
			int cc = read(new_fd,cmd,sizeof(cmd));
			if(cc <= 0)
			{
				perror("folder in ");
				return;
			}
			char pathe[1024];
            		int len = snprintf(pathe, sizeof(pathe)-1, "%s/%s", path, cmd);
            		pathe[len] = 0;
			int fdss , redd ;
			if((fdss = open(pathe,O_CREAT | O_WRONLY | O_TRUNC,0600)) < 0 || (fdss = open(pathe,O_RDONLY,0755)) < 0)
				{printf("fdss  %d\n",fdss);close(fdss);
				written = write(new_fd,"WRONG",sizeof("WRONG"));if(written <= 0)
			{perror("write");return;}read(new_fd,cmd,sizeof cmd);
				continue;}
			written = write(new_fd,"DONE",sizeof("DONE"));if(written < 0)
			{perror("write");return;}
			close(fdss);
			strcpy(ret,pathe);			
			break;
			char cwd[1024];
     			if (getcwd(cwd, sizeof(cwd)) != NULL)
       			   	 fprintf(stdout, "Current working dir: %s/%s\n", cwd,cmd);
     		  	else	
     		    		  perror("getcwd() error");
			break;
		}
		else
			printf("%s\n",path);
	}
    return ;

}

void listthedir(int new_fd ,char ret[1024]){
	
	//int num = 0;
	strcpy(ret,"");
	int ti = 0;
	char cmd[1024];
	int rr;
	char booth[1024];					
	for(;ti < num;ti++){
		int len = snprintf(booth,sizeof(booth)-1,"   {%s}$",loc[ti]);
		booth[len] = 0;
		int written = write(new_fd,booth,strlen(booth));
		if(written <= 0)
			{perror("write");return;}			
	}
	int size_done = sizeof("DONE$");
	int written = write(new_fd,"DONE$",size_done);
		if(written <= 0)
			{perror("write");return;}
	char path[1024];
	//strcpy(path,cmd);
	while(1){
		//printf("Enter the path\n");
		memset(path,0,sizeof path);
		int bytes = read(new_fd,path,sizeof path);//scan the cmd
		printf("Dire   %s\n",path);
		if(bytes <= 0)
		{
			perror("Error in listthedir");		
			return;
		}
		ti = 0;
		rr = 0;
		for(;ti < num;ti++)
		{
			if(!strcmp(path,loc[ti]))
				{rr = 1;break;}				
		}
		if(rr == 1)
			{written = write(new_fd,"DONE",sizeof("DONE"));
			if(written <= 0)
			{perror("write");return;}
			break;}
		written = write(new_fd,"boo",sizeof("boo"));
		if(written <= 0)
			{perror("write");return;}
	}

/*

	while(1){
		//printf("Enter the path\n");
		int bytes = read(new_fd,cmd,sizeof cmd);//scan the cmd
		if(bytes <= 0)
		{
			perror("Error in listthedir");		
			return ;
		}
		ti = 0;
		rr = 0;
		for(;ti < num;ti++)
		{
			if(!strcmp(cmd,loc[ti]))
				{rr = 1;break;}				
		}
		if(rr == 1)break;
	}
*/	
	read(new_fd,cmd,sizeof cmd);
	char folder[1024][1024];
	int level = 0;
	while(1){
		int p = 0;
		printf("goining to listdir\n");
		listdir(path, level ,folder,&p,new_fd);
		printf("Enter the command\n");	
		char cmd[1024];
		memset(cmd,0,sizeof cmd);
		int bytes = read(new_fd,cmd,sizeof cmd);//scan the cmd
		printf("%s        booooo\n",cmd);
		if(bytes <= 0)
		{
			perror("Error in listthedir");		
			return ;
		}
		if(!strcmp(cmd,"back"))
		{
			level = level - 1;
			char c[2];
			c[0] = 65+level;
			c[1] ='\0';
			printf("%s",c);
			if((bytes = write(new_fd,c,strlen(c))) <= 0){perror("burrr 0");return;}
			if((bytes = read(new_fd,cmd,sizeof cmd))<= 0) {perror("here 1");return;}
			memset(cmd,0,sizeof cmd);
			if(level < 0)
			{					
				ti = 0;
				for(;ti <num;ti++){
					int len = snprintf(booth,sizeof(booth)-1,"   {%s}$",loc[ti]);
					booth[len] = 0;
					written  = write(new_fd,booth,strlen(booth));if(written <= 0)
			{perror("write");return;}			
				}
				written = write(new_fd,"DONE$",sizeof("DONE$"));if(written <= 0)
			{perror("write");return;}
				//write(new_fd,"-1",sizeof("-1"));
				while(1){
					printf("Enter the path\n");
					memset(path,0,sizeof path);
					int bytes = read(new_fd,path,sizeof path);//scan the cmd
					printf("Dire   %s\n",path);
					if(bytes <= 0)
					{
						perror("Error in listthedir");		
						return;
					}
					ti = 0;
					rr = 0;
					for(;ti < num;ti++)
					{
						if(!strcmp(path,loc[ti]))
							{rr = 1;break;}				
					}
					if(rr == 1)
						{written = write(new_fd,"DONE",sizeof("DONE"));if(written <= 0)
			{perror("write");return;}break;}
					written = write(new_fd,"boo",sizeof("boo"));if(written <= 0)
			{perror("write");return;}
				}
				level += 1;
				read(new_fd,cmd,sizeof(cmd));
			}
			else
			{
				//write(new_fd,"0",sizeof("0"));
				int tp = strlen(path)-1;
				while(tp--)
				{
					if(path[tp] == '/')
						{path[tp]='\0';break;}			
				}
			}
			continue;
		}
		else if(!strcmp(cmd,"folder")){
			memset(cmd,0,sizeof cmd);
			int cc = read(new_fd,cmd,sizeof(cmd));
			if(cc <= 0 ) {perror("thuus 0"); return ;}		
			int flag = 0;
			int i = 0;
			for(i = 0 ; i < p ;i++){
				if(!strcmp(cmd,folder[i])){
					flag = 1;
					break;
				}
			}
			if(flag)
			{
				char pathe[1024];
				printf("DIR BEFORE   %s\n",path);
		    		int len = snprintf(pathe, sizeof(pathe)-1, "%s/%s", path, cmd);
		    		pathe[len] = 0;
				strcpy(path,pathe);
				printf("DIR AFTER   %s\n",path);
				level += 1;
			}
		}
		else if(!strcmp(cmd,"file"))
		{
			memset(cmd,0,sizeof cmd);
			int cc=read(new_fd,cmd,sizeof cmd);
			if(cc <= 0){perror("file yuuu ");return;}
			char pathe[1024];
            		int len = snprintf(pathe, sizeof(pathe)-1, "%s/%s", path, cmd);
            		pathe[len] = 0;
			int fdss , redd ;
			if((fdss = open(pathe,O_RDONLY,0755)) < 0 || (redd = read(fdss,cmd,sizeof cmd)) < 0)
				{close(fdss);
				written = write(new_fd,"WRONG",sizeof("WRONG"));if(written <= 0)
			{perror("write");return;}read(new_fd,cmd,sizeof cmd);
				continue;}
			written = write(new_fd,"DONE",sizeof("DONE"));if(written < 0)
			{perror("write");return;}
			close(fdss);
			strcpy(ret,pathe);			
			break;
			char cwd[1024];
     			if (getcwd(cwd, sizeof(cwd)) != NULL)
       			   	 fprintf(stdout, "Current working dir: %s/%s\n", cwd,cmd);
     		  	else	
     		    		  perror("getcwd() error");
			break;
		}
		else
			printf("%s\n",path);
	}
    return ;
}

int
duration (struct timeval *start, struct timeval *stop, struct timeval *delta)
{
  suseconds_t microstart, microstop, microdelta;

  microstart = (suseconds_t) (100000 * (start->tv_sec)) + start->tv_usec;
  microstop = (suseconds_t) (100000 * (stop->tv_sec)) + stop->tv_usec;
  microdelta = microstop - microstart;

  delta->tv_usec = microdelta % 100000;
  delta->tv_sec = (time_t) (microdelta / 100000);

  if ((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
    return -1;
  else
    return 0;
}

void
list_send (int new_fd)
{

  char buffer[BUFFERT];
  int file_fd = open ("ls.txt", O_RDONLY, 0755);

  while (1)
    {

      // Read data into buffer.  We may not have enough to fill up buffer, so we
      // store how many bytes were actually read in bytes_read.

      int bytes_read = read (file_fd, buffer, sizeof (buffer));
      if (bytes_read == 0)	// We're done reading from the file
	break;

      if (bytes_read < 0)
	{
	  perror ("reading from file");
	  return;
	}

      // You need a loop for the write, because not all of the data may be written
      // in one call; write will return how many bytes were written. p keeps
      // track of where in the buffer we are, while we decrement bytes_read
      // to keep track of how many bytes are left to write.

      void *p = buffer;
      while (bytes_read > 0)
	{
	  int bytes_written = write (new_fd, p, bytes_read);
	  if (bytes_written <= 0)
	    {
	      perror ("sending");
	    }
	  bytes_read -= bytes_written;
	  p += bytes_written;
	}
    }

//close(new_fd);

}


void
download (int new_fd)
{
  struct timeval start, stop, delta;
  char buffer[BUFFERT];
  char file_name[1024];
  off_t sz;
  struct stat file_size;
  listthedir(new_fd,file_name);
  printf("the file is %s\n",file_name);
  if(!strcmp(file_name,""))
	return ;
  /*struct addrinfo two;
     memset(&two , 0 , sizeof two);
     two.ai_family = AF_UNSPEC;
     two.ai_socktype = SOCK_STREAM;
     two.ai_flags = AI_PASSIVE;   // use my IP
     int tran_fd = socket(two->ai_family,two->ai_socktype);
   */
  //printf("name %s",file_name);

	if(search(file_name,"u"))
	{
		printf("file in use\n");
		return ;	
	}
	memset(buffer,0,sizeof buffer);
	read(new_fd,buffer,sizeof buffer);
	printf("GOAAAS          %s\n",buffer);
	memset(buffer,0,sizeof buffer);

	int writt;
	if((writt = write(new_fd,file_name,strlen(file_name))) <= 0)
		return ;
	else
		printf("dasdfasdfasdf      %d\n",writt);
	memset(buffer,0,sizeof buffer);
	read(new_fd,buffer,sizeof buffer);
	printf("BANANA          %s\n",buffer);
	memset(buffer,0,sizeof buffer);
  insert(file_name,"d");
  int file_fd = open (file_name, O_RDONLY, 0755);
  if (file_fd == -1)
    {
      perror ("file");
      return;
    }

  if (stat (file_name, &file_size) == -1)
    {
      perror ("stat fail");
      return;
    }
  else
    sz = file_size.st_size;
/*int l = sizeof(struct sockaddr_in);
int count = 0;
int m;
int n=read(file_fd,buf,BUFFERT);
	while(n){
		if(n==-1){
			perror("read fails");
			return;
		}
		m=sendto(new_fd,buf,n,0,(struct sockaddr*)&servinfo,l);
		if(m==-1){
			perror("send error");
			return;
		}
		count+=m;
		//fprintf(stdout,"----\n%s\n----\n",buf);
		bzero(buf,BUFFERT);
        n=read(file_fd,buf,BUFFERT);
	}
	
	m=sendto(new_fd,buf,0,0,(struct sockaddr*)&servinfo,l);

	printf("%d\n",count);
*/
/////////////////////////

  long long int total = 0;


	//write(new_fd,)


  gettimeofday (&start, NULL);
  while (1)
    {

      // Read data into buffer.  We may not have enough to fill up buffer, so we
      // store how many bytes were actually read in bytes_read.

      int bytes_read = read (file_fd, buffer, sizeof (buffer));
      if (bytes_read == 0)	// We're done reading from the file
	break;

      if (bytes_read < 0)
	{
	  perror ("reading from file");
	  return;
	}

      // You need a loop for the write, because not all of the data may be written
      // in one call; write will return how many bytes were written. p keeps
      // track of where in the buffer we are, while we decrement bytes_read
      // to keep track of how many bytes are left to write.

      void *p = buffer;
      while (bytes_read > 0)
	{
	  int bytes_written = write (new_fd, p, bytes_read);
	  if (bytes_written <= 0)
	    {
	      perror ("sending");
	      break;
	    }
	  bytes_read -= bytes_written;
	  p += bytes_written;
	  total += bytes_written;
	}
    }
	
	shutdown(new_fd,SHUT_WR);

	delete(file_name,"d");
  gettimeofday (&stop, NULL);
  duration (&start, &stop, &delta);
  printf ("Total data sent is : %lld\n", total);
  printf ("File size is : %jd\n", (intmax_t) sz);
  printf ("Total time taken is  : %jd.%jd \n", (intmax_t) delta.tv_sec,
	  (intmax_t) delta.tv_usec);

	memset(buffer,0,sizeof buffer);
	int bb = read(new_fd,buffer , sizeof buffer);
	if(bb <= 0)
	{
		perror("Client lost");
		return;
	}
	printf("buffer  %s\n",buffer);
	if(!strcmp(buffer,"DONE"))
	printf("BANANAS\n");
	else
	printf("MANGOES\n");
}

void
upload (int new_fd)
{
//download(new_fd);
	printf("Started upload\n");
	//shutdown(new_fd,SHUT_WR);
	//return;
  struct timeval start, stop, delta;
  char buffer[1024];
  char file_name[1024];

	listthedirupload(new_fd,file_name);

	printf("the file is %s\n",file_name);
  if(!strcmp(file_name,""))
	return ;

	read(new_fd,buffer,sizeof buffer); //the  dud 
	write(new_fd,"dud",sizeof("dud")); // reverse dud
	

  /*int bytes = recv (new_fd, file_name, sizeof file_name, 0);
  if (bytes <= 0)
    {
      perror ("recieving in upload 1");
      return;
    }
	printf("FILE NAME IS %s\n",file_name);
	return;*/
 // printf ("Incoming upload file %s . Save as ?", file_name);
  //scanf ("%s", file_name);
  if(search(file_name,"u") || search(file_name,"d"))
  {
	printf("File in use \n");
	return ;
  }
  insert(file_name,"u");
  int file_fd = open (file_name, O_CREAT | O_WRONLY | O_TRUNC, 0600);
  if (file_fd == -1)
    {
      perror ("recieving in upload 2");
      return;
    }
  printf ("Downloading in Progress...\n");
  long long int total = 0;
  gettimeofday (&start, NULL);
	
  while (1)
    {
	memset(buffer,0,sizeof buffer);
      int bytes = recv (new_fd, buffer, sizeof buffer, 0);
	//printf("Recieved is %s\n",buffer);
      //buffer[bytes] = '\0'; 
      //printf("%d\n" , bytes);       
      if (bytes == 0)
	{
	  write(new_fd,"DONE",sizeof("DONE"));
	  delete(file_name,"u");
	  gettimeofday (&stop, NULL);
	  duration (&start, &stop, &delta);
	  printf ("Total time taken is  : %jd.%jd \n",
		  (intmax_t) delta.tv_sec, (intmax_t) delta.tv_usec);
	  printf ("Transered total of %lld\n", total);
	  puts ("Done\n");
	  return;
	}
      if (bytes < 0)
	{
	  perror ("Downloading Terminated");
	  return;
	}
      int written = write (file_fd, buffer, bytes);
	if(written <= 0)
			{perror("write");return;}
      total += bytes;
     // memset (buffer, 0, sizeof buffer);
    }
}


void *
client_connection (void *args)
{

  printf ("Success in thread\n");
  banana *actual_args = args;

  int new_fd = actual_args->socketfd;

	/***************** STUFF THAT IS TO BE SENT IS WRITTEN HERE ***************************/

  char cmd[3];

  int bytes;
  if ((bytes = recv (new_fd, cmd, 3, 0)) == -1)
    perror ("recieve");
  cmd[bytes] = '\0';
/*
  if (!strcmp (cmd, "ls"))
    {
      system ("/home/ubuntu/Documents/list.sh");
      /*pthread_t tid;
         int err = pthread_create(&(tid), NULL, &list_send, var);
         if(err == -1)
         {
         perror("thread");
         } ///
      list_send (new_fd);
    }
  else*/ if (!strcmp (cmd, "d"))
    {
      printf ("hello\n");
      download (new_fd);
      //list_send();
    }
  else if (!strcmp (cmd, "u"))
    {
	printf("bello\n");
	//download(new_fd);
      upload (new_fd);
    }
  else
    {
      printf ("Invalid command\n");
    }

  close (new_fd);		// parent doesn't need this
	printf("BYE bYE\n");
  //free(actual_args);

}

int
main (void)
{
	printf("How many locations \n");
	//int num = 0;
	scanf("%d",&num);
	int ti = 0;
	for(;ti < num ;ti++){
		scanf("%s",loc[ti]);
		DIR *dir;
   		 if (!(dir = opendir(loc[ti])))
     		 {printf("Npt found\n");  ti--;}
	}
	h = NULL;
    temp = temp1 = NULL;

  memset (&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;	// use my IP


  if ((rv = getaddrinfo (NULL, PORT, &hints, &servinfo)) != 0)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (rv));
      return 1;
    }

// loop through all the results and bind to the first we can

  for (p = servinfo; p != NULL; p = p->ai_next)
    {
      if ((sockfd = socket (p->ai_family, p->ai_socktype,
			    p->ai_protocol)) == -1)
	{
	  perror ("server: socket");
	  continue;
	}
      if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
		      sizeof (int)) == -1)
	{
	  perror ("setsockopt");
	  exit (1);
	}
      if (bind (sockfd, p->ai_addr, p->ai_addrlen) == -1)
	{
	  close (sockfd);
	  perror ("server: bind");
	  continue;
	}
      break;
    }

  if (p == NULL)
    {
      fprintf (stderr, "server: failed to bind\n");
      return 2;
    }

  freeaddrinfo (servinfo);	// all done with this structure

  if (listen (sockfd, BACKLOG) == -1)
    {
      perror ("listen");
      exit (1);
    }
/*
  sa.sa_handler = sigchld_handler;	// reap all dead processes
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction (SIGCHLD, &sa, NULL) == -1)
    {
      perror ("sigaction");
      exit (1);
    }
*/

 // system (PERMISSION);
  while (1)
    {				// main accept() loop

      sin_size = sizeof their_addr;	//for size of struct

      printf ("server: waiting for connections...\n");

      int new_fd =
	accept (sockfd, (struct sockaddr *) &their_addr, &sin_size);

      if (new_fd == -1)
	{
	  perror ("accept");
	  continue;
	}

      banana *var = malloc (sizeof *var);
      var->socketfd = new_fd;

      inet_ntop (their_addr.ss_family, get_in_addr ((struct sockaddr *) &their_addr), s, sizeof s);	// convert the ip of client to string.

      printf ("server: got connection from %s\n", s);
      pthread_t tid;
      int err = pthread_create (&(tid), NULL, &client_connection, var);
      if (err == -1)
	{
	  perror ("thread");
	}
    }

  return 0;

}
