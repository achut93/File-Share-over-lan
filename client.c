/*
** client.c -- a stream socket client
*
*  Author : Achut Nandam
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define PORT "3490"		// the port client will be connecting to
#define MAXDATASIZE 1000001	// max number of bytes we can get at once
#define PERMISSION "/home/curiosity/Documents/cns/permission.sh"
#define BUFFERT 1024

/************** Variables ********************/

int sockfd;
struct addrinfo hints, *servinfo, *p;
struct sockaddr_in clt;
int rv;
char s[INET6_ADDRSTRLEN];
char file_name_download[1024];
char file_name[1024];
/*********************************************/

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
recvall (int s, char *buf, int len)
{
  int total = 0;		// how many bytes we've sent
  int bytesleft = len;		// how many we have left to send
  int n;

  while (total < len)
    {
      n = recv (s, buf + total, bytesleft, 0);
      if (n == -1 || n == 0)
	{
	  break;
	}
      total += n;
      bytesleft -= n;
    }
  len = total;			// return number actually sent here
  return n == -1 ? -1 : 0;	// return -1 on failure, 0 on success
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
fill ()
{

  memset (&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

}

void
list ()
{
  char buffer[BUFFERT];
  long long int total = 0;
  while (1)
    {
      int bytes = recv (sockfd, buffer, sizeof (buffer), 0);
      buffer[bytes] = '\0';
      if (bytes < 0)
	{
	  perror ("recieveing");
	  exit (1);
	}
      if (bytes == 0)
	{
	  printf ("Completed Recieving\n");
	  return;
	}
      total += bytes;
      printf ("%s \n", buffer);
    }
  printf ("%lld\n", total);
}

void
download ()
{
  char buffer[BUFFERT];
	int byte = 0;
	char rec[1024];
	int idx = 0;
	int l =0;
	int fl = 0;
	while(1){
		byte = read(sockfd,buffer,sizeof buffer);
		if(byte <= 0)
		{
			perror("download 1");
			exit(1);	
		}
		for(l = 0; l < byte ;l++,idx++)
		{
			if(buffer[l] == '$')
			{
				rec[idx] = 0;
				if(strcmp(rec,"DONE"))
					{idx = -1;printf("%s\n",rec);}
				else {fl = 1;break;}
			}
			else{
				rec[idx] = buffer[l];
			}		
		}
		if(fl == 1)
			break;
	}
	idx = 0;
	l = 0;
	fl = byte  = 0;
	//printf("banana\n");
	char cmd[1024];	
	while(1){
		printf("Enter the directory\n");
		scanf("%s",cmd);
		write(sockfd,cmd,strlen(cmd));
		memset(cmd,0,sizeof cmd);
		byte = read(sockfd,cmd,sizeof cmd);
		if(byte <= 0)
		{
			perror("download 0");
			exit(1);	
		}
		if(!strcmp(cmd,"DONE"))
			break;
		//printf("booob \n");
	}
	write(sockfd,"dud",sizeof("dud"));
	
	int level = 0;
	while(1){
		printf("reading...\n");
		while(1){
			memset(buffer,0,sizeof buffer);
			byte = read(sockfd,buffer,sizeof buffer);
			if(byte <= 0)
			{
				perror("download 1");
				exit(1);	
			}
			//printf("%d   %s\n",byte , buffer);
			for(l = 0; l < byte ;l++,idx++)
			{
				if(buffer[l] == '$')
				{
					rec[idx] = 0;
					if(strcmp(rec,"DONE"))
						{idx = -1;printf("%s\n",rec);}
					else {fl = 1;break;}
				}
				else{
					rec[idx] = buffer[l];
				}		
			}
			if(fl == 1)
				break;
		}
		idx = 0;
		l = 0;
		fl = byte  = 0;
		printf("file or folder or back\n");
		scanf("%s",cmd);
		if(!strcmp(cmd,"back"))
		{	
			write(sockfd,cmd,strlen(cmd));
			//level -= 1;
			memset(cmd,0,sizeof cmd);
			read(sockfd,cmd,sizeof cmd);
			printf("%s\n",cmd);
			level = cmd[0]-'A';
			write(sockfd,"dud",sizeof("dud"));
			printf("level   %d\n",level);
			if(level < 0)
			{
				while(1){
					memset(buffer,0,sizeof buffer);
					byte = read(sockfd,buffer,sizeof buffer);
					if(byte <= 0)
					{
						perror("download 1");
						exit(1);	
					}
					//printf("%d   %s\n",byte , buffer);
					for(l = 0; l < byte ;l++,idx++)
					{
						if(buffer[l] == '$')
						{
							rec[idx] = 0;
							if(strcmp(rec,"DONE"))
								{idx = -1;printf("%s\n",rec);}
							else {fl = 1;break;}
						}
						else{
							rec[idx] = buffer[l];
						}		
					}
					if(fl == 1)
						break;
				}
				idx = 0;
				l = 0;
				fl = byte  = 0;	
				while(1){
					printf("Enter the path\n");
					scanf("%s",cmd);
					write(sockfd,cmd,strlen(cmd));
					memset(cmd,0,sizeof cmd);
					read(sockfd,cmd,sizeof cmd);
					if(!strcmp(cmd,"DONE"))
						break;
					//printf("booob \n");
				}
				level += 1;	
				write(sockfd,"dud",sizeof("dud"));
			}
		}
		else if(!strcmp(cmd,"folder")){
			write(sockfd,cmd,strlen(cmd));
			printf("Enter the folder name\n");	
			scanf("%s",cmd);
			write(sockfd,cmd,strlen(cmd));
			level += 1;
		}
		else if (!strcmp(cmd,"file")){
			write(sockfd,cmd,strlen(cmd));
			printf("Enter the file name\n");	
			scanf("%s",cmd);
			//printf("CMD   %s\n",cmd);
			write(sockfd,cmd,strlen(cmd));
			memset(cmd,0,sizeof cmd);
			byte = read(sockfd,cmd,sizeof cmd);	
			write(sockfd,"dud",sizeof("dud"));	
			if(byte <= 0)
				{perror("read");exit(1);}
			printf("FILE    %s\n",cmd);
			if(!strcmp(cmd,"DONE"))			
			break;
		}
		else{
			write(sockfd,cmd,strlen(cmd));
		}
	}
	printf("Dummy\n");
	//memset(buffer,0,sizeof buffer);
	//int b = read(sockfd,buffer,sizeof(buffer));
	//  printf("%s\n",buffer);
	//printf ("Enter the file you want to download \n");
 // char file_name[1024];
  //scanf ("%s", file_name);
  //write (sockfd, file_name, sizeof file_name);
	write(sockfd,"dud",sizeof("dud"));
	int by ;
	if((by = read(sockfd,file_name_download,1024)) <= 0)
		{printf("DOOOO\n");exit(1);}
	//write(sockfd,"dud",sizeof("dud"));///////////////////////////////////////////
	printf("location  %s\n",file_name_download);
  printf ("Enter the file you want to save as \n");
  scanf ("%s", file_name);
  int file_fd = open (file_name, O_CREAT | O_WRONLY | O_TRUNC, 0600);
  if (file_fd == -1)
    {
      perror ("file");
      exit (1);
    }
  printf ("Downloading in Progress...\n");
  
	  


  int total = 0;
  while (1)
    {
	memset(buffer,0,sizeof buffer);
      int bytes = recv (sockfd, buffer, sizeof (buffer), 0);
	//printf("%d    %s",bytes, buffer);
      //buffer[bytes] = '\0'; 
      //printf("%d\n" , bytes);       
      if (bytes == 0)
	{
	  printf ("Transered total of %d\n", total);
	  puts ("Done\n");
		write(sockfd,"DONE" , sizeof("DONE"));
	  return;
	}
      if (bytes < 0)
	{
	  perror ("Downloading Terminated");
	  exit (1);
	}
      write (file_fd, buffer, bytes);
      total += bytes;
    }
/*
int l = sizeof(struct sockaddr_in);
    int count = 0;
	bzero(&buf,BUFFERT);
   long long n = recvfrom(sockfd,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	while(n){
		printf("%lld of data received \n",n);
		if(n==-1){
			perror("read fails");
			exit(1);
		}
		count+=n;
		write(file_fd,buf,n);
		bzero(buf,BUFFERT);
        n=recvfrom(sockfd,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	}
	printf("%d\n",count);
*/
}

void
upload ()
{
  struct timeval start, stop, delta;
  off_t sz;
  struct stat file_size;
  char buffer[1024];
	//char file_name[1024];
	//download();
	printf("Started upload\n");
	//printf("ENter the file name to upload\n");
	//scnaf("%s",file_name);
	//printf("FILE NAME    %s\n",file_name);
	//int status = system(file_name);
	//printf("APPLE %d   \n",status);
	//exit(1);
while(1){
  printf ("Enter the file to upload\n");
  scanf ("%s", file_name);
	int ttt;
	if((ttt = open(file_name,O_RDONLY)) >= 0)
		{close(ttt);break;}
}
printf("Select where to upload\n");
        int byte = 0;
	char rec[1024];
	int idx = 0;
	int l =0;
	int fl = 0;
	while(1){
		byte = read(sockfd,buffer,sizeof buffer);
		if(byte <= 0)
		{
			perror("download 1");
			exit(1);	
		}
		//printf("bytes   %d    %s\n",byte,buffer);
		for(l = 0; l < byte ;l++,idx++)
		{
			if(buffer[l] == '$')
			{
				rec[idx] = 0;
				if(strcmp(rec,"DONE"))
					{idx = -1;printf("%s\n",rec);}
				else {fl = 1;break;}
			}
			else{
				rec[idx] = buffer[l];
			}		
		}
		if(fl == 1)
			break;
	}
	idx = 0;
	l = 0;
	fl = byte  = 0;
	//printf("banana\n");
	char cmd[1024];	
	while(1){
		printf("Enter the directory\n");
		scanf("%s",cmd);
		write(sockfd,cmd,strlen(cmd));
		memset(cmd,0,sizeof cmd);
		byte = read(sockfd,cmd,sizeof cmd);
		if(byte <= 0)
		{
			perror("download 0");
			exit(1);	
		}
		if(!strcmp(cmd,"DONE"))
			break;
		//printf("booo0b \n");
	}
	write(sockfd,"dud",sizeof("dud"));
	
	int level = 0;
	while(1){
		printf("reading...\n");
		while(1){
			memset(buffer,0,sizeof buffer);
			byte = read(sockfd,buffer,sizeof buffer);
			if(byte <= 0)
			{
				perror("download 1");
				exit(1);	
			}
			//printf("%d   %s\n",byte , buffer);
			for(l = 0; l < byte ;l++,idx++)
			{
				if(buffer[l] == '$')
				{
					rec[idx] = 0;
					if(strcmp(rec,"DONE"))
						{idx = -1;printf("%s\n",rec);}
					else {fl = 1;break;}
				}
				else{
					rec[idx] = buffer[l];
				}		
			}
			if(fl == 1)
				break;
		}
		idx = 0;
		l = 0;
		fl = byte  = 0;
		printf("file or folder or back\n");
		scanf("%s",cmd);
		if(!strcmp(cmd,"back"))
		{	
			write(sockfd,cmd,strlen(cmd));
			//level -= 1;
			memset(cmd,0,sizeof cmd);
			read(sockfd,cmd,sizeof cmd);
			printf("%s\n",cmd);
			level = cmd[0]-'A';
			write(sockfd,"dud",sizeof("dud"));
			printf("level   %d\n",level);
			if(level < 0)
			{
				while(1){
					memset(buffer,0,sizeof buffer);
					byte = read(sockfd,buffer,sizeof buffer);
					if(byte <= 0)
					{
						perror("download 1");
						exit(1);	
					}
					//printf("%d   %s\n",byte , buffer);
					for(l = 0; l < byte ;l++,idx++)
					{
						if(buffer[l] == '$')
						{
							rec[idx] = 0;
							if(strcmp(rec,"DONE"))
								{idx = -1;printf("%s\n",rec);}
							else {fl = 1;break;}
						}
						else{
							rec[idx] = buffer[l];
						}		
					}
					if(fl == 1)
						break;
				}
				idx = 0;
				l = 0;
				fl = byte  = 0;	
				while(1){
					printf("Enter the path\n");
					scanf("%s",cmd);
					write(sockfd,cmd,strlen(cmd));
					memset(cmd,0,sizeof cmd);
					read(sockfd,cmd,sizeof cmd);
					if(!strcmp(cmd,"DONE"))
						break;
					//printf("booob \n");
				}
				level += 1;	
				write(sockfd,"dud",sizeof("dud"));
			}
		}
		else if(!strcmp(cmd,"folder")){
			write(sockfd,cmd,strlen(cmd));
			printf("Enter the folder name\n");	
			scanf("%s",cmd);
			write(sockfd,cmd,strlen(cmd));
			level += 1;
		}
		else if (!strcmp(cmd,"file")){
			write(sockfd,cmd,strlen(cmd));
			printf("Enter the file name\n");	
			scanf("%s",cmd);
			//printf("CMD   %s\n",cmd);
			write(sockfd,cmd,strlen(cmd));
			memset(cmd,0,sizeof cmd);
			byte = read(sockfd,cmd,sizeof cmd);	
			write(sockfd,"dud",sizeof("dud"));	
			if(byte <= 0)
				{perror("read");exit(1);}
			printf("FILE    %s\n",cmd);
			if(!strcmp(cmd,"DONE"))			
			break;
		}
		else{
			write(sockfd,cmd,strlen(cmd));
		}
	}

	printf("Out of the loop \n");

  //Reading and sending the file 
  int file_fd = open (file_name, O_RDONLY);
  if (file_fd == -1)
    {
      perror ("opening file in upload");
      return;
    }
  if (stat (file_name, &file_size) == -1)
    {
      perror ("stat fail");
      return;
    }
  else
    sz = file_size.st_size;
	memset(buffer,0,sizeof buffer);
	read(sockfd,buffer,sizeof buffer);
	printf("reverse dud   %s\n",buffer);
  //write (sockfd, file_name, sizeof (file_name));
  long long int total = 0;
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
	  perror ("reading from file in upload");
	  return;
	}

      // You need a loop for the write, because not all of the data may be written
      // in one call; write will return how many bytes were written. p keeps
      // track of where in the buffer we are, while we decrement bytes_read
      // to keep track of how many bytes are left to write.

      void *p = buffer;
      while (bytes_read > 0)
	{
	  int bytes_written = write (sockfd, p, bytes_read);
	  if (bytes_written <= 0)
	    {
	      perror ("sending to server in upload");
	    }
	  bytes_read -= bytes_written;
	  p += bytes_written;
	  total += bytes_written;
	}
    }
	shutdown(sockfd,SHUT_WR);
  gettimeofday (&stop, NULL);
  duration (&start, &stop, &delta);
  printf ("Total data sent is : %lld\n", total);
  printf ("File size is : %jd\n", (intmax_t) sz);
  printf ("Total time taken is  : %jd.%jd \n", (intmax_t) delta.tv_sec,
	  (intmax_t) delta.tv_usec);
	memset(buffer,0,sizeof buffer);
	int cc = read(sockfd,buffer,sizeof buffer);
	printf("bffff  %s\n",buffer);
	if(cc > 0 && !strcmp(buffer,"DONE"))
		printf("BANAAS \n");
	else
		printf("GOASS \n");
}


int
main (int argc, char *argv[])
{
  //system (PERMISSION);
  if (argc != 2)
    {
      fprintf (stderr, "usage: client hostname\n");
      exit (1);
    }

  fill ();

  if ((rv = getaddrinfo (argv[1], PORT, &hints, &servinfo)) != 0)//htons required ??
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (rv));
      return 1;
    }

// loop through all the results and connect to the first we can

  for (p = servinfo; p != NULL; p = p->ai_next)
    {
      if ((sockfd = socket (p->ai_family, p->ai_socktype,
			    p->ai_protocol)) == -1)
	{
	  perror ("client: socket");
	  continue;
	}
      if (connect (sockfd, p->ai_addr, p->ai_addrlen) == -1)//htonl required ??
	{
	  close (sockfd);
	  perror ("client: connect");
	  continue;
	}
      break;
    }

  if (p == NULL)
    {
      fprintf (stderr, "client: failed to connect\n");
      return 2;
    }

  inet_ntop (p->ai_family, get_in_addr ((struct sockaddr *) p->ai_addr),
	     s, sizeof s);
  printf ("client: connecting to %s\n", s);

  freeaddrinfo (servinfo);	// all done with this structure

  printf ("d download \nu upload\nq quit\nEnter a command\n");
  char cmd[3];
  scanf ("%s", cmd);
/*
  if (!strcmp (cmd, "ls"))
    {
      if (send (sockfd, cmd, sizeof cmd, 0) == -1)
	{
	  perror ("sending in ls ");
	  exit (1);
	}
      list ();
    }
  else */if (!strcmp (cmd, "d"))
    {
      if (send (sockfd, cmd, sizeof cmd, 0) == -1)
	{
	  perror ("sending in d");
	  exit (1);
	}
      printf ("hello\n");
      download ();
    }
  else if (!strcmp (cmd, "u"))
    {
      if (send (sockfd, cmd, sizeof cmd, 0) == -1)
	{
	  perror ("sending in u");
	  exit (1);
	}
	printf("bello\n");
	//download();
      upload ();
    }

  printf ("Closing Connection\n");
  close (sockfd);

  return 0;

}
