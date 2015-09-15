/*
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

int main(){

DIR *dir;
struct dirent *ent;
if ((dir = opendir ("/home/ubuntu/Documents")) != NULL) {
  ///* print all the files and directories within directory ///
  while ((ent = readdir (dir)) != NULL) {
    printf ("%s\n", ent->d_name);
  }
  closedir (dir);
} else {
  ///* could not open directory /
  perror ("");
  return EXIT_FAILURE;
}
return 0;
}
*/

/*
To know the directory of a current dir 

char cwd[1024];
     		  if (getcwd(cwd, sizeof(cwd)) != NULL)
       			    fprintf(stdout, "Current working dir: %s\n", cwd);
     		  else
     		      perror("getcwd() error");

*/
#include <fcntl.h>
 
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <libgen.h>
#include <unistd.h>

void listdir(const char *name, int level ,char folder[1024][1024] ,int *p)
{
	//printf("%s\n",name);
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
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            printf("%*s[%s]\n", level*2, "", entry->d_name);
		strcpy(folder[*p], (char *)entry->d_name);
		*p += 1;
          //  listdir(path, level + 1);
        }
        else
            {
		printf("%*s- %s\n", level*2, "", entry->d_name);		
	    }
    } while (entry = readdir(dir));
    closedir(dir);
}

int main(void)
{
	char loc[1024][1024];
	printf("How many locations \n");
	int num = 0;
	scanf("%d",&num);
	int ti = 0;
	for(;ti < num ;ti++){
		scanf("%s",loc[ti]);
	}
	char cmd[1024];
	int rr;
	while(1){
		printf("Enter the path\n");
		scanf("%s",cmd);
		ti = 0;
		rr = 0;
		for(;ti < num;ti++)
		{
			if(!strcmp(cmd,loc[ti]))
				{rr = 1;break;}				
		}
		if(rr == 1)break;
	}
	char path[1024];
	strcpy(path,cmd);
	char folder[1024][1024];
	int level = 0;
	while(1){
		int p = 0;
		listdir(path, level ,folder,&p);
		printf("Enter the command\n");	
		char cmd[1024];
		scanf("%s",cmd);
		if(!strcmp(cmd,"back"))
		{
			level = level - 1;
			if(level < 0)
			{					
				ti = 0;
				for(;ti <num;ti++){
					printf("   {%s}\n",loc[ti]);			
				}
				while(1){
					printf("Enter the path\n");
					scanf("%s",path);
					ti = 0;
					rr = 0;
					for(;ti < num;ti++)
					{
						if(!strcmp(path,loc[ti]))
							{rr = 1;break;}				
					}
					if(rr == 1)break;
				}
				level += 1;
			}
			else
			{
				int tp = strlen(path)-1;
				while(tp--)
				{
					if(path[tp] == '/')
						{path[tp+1]='\0';break;}			
				}
			}
			continue;
		}
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
            		int len = snprintf(pathe, sizeof(pathe)-1, "%s/%s", path, cmd);
            		pathe[len] = 0;
			strcpy(path,pathe);
			level += 1;
		}
		else
		{
			char pathe[1024];
            		int len = snprintf(pathe, sizeof(pathe)-1, "%s/%s", path, cmd);
            		pathe[len] = 0;
			int fdss ;
			if((fdss = open(pathe,O_RDONLY)) < 0)
				{printf("Entered wrong\n");continue;}
			close(fdss);
			char cwd[1024];
     			if (getcwd(cwd, sizeof(cwd)) != NULL)
       			   	 fprintf(stdout, "Current working dir: %s/%s\n", cwd,cmd);
     		  	else	
     		    		  perror("getcwd() error");
			break;
		}
	}
    return 0;
}
