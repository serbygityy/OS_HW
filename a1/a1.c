#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define ERROR_PATH 1
#define ERROR_FILTER 2
#define MAX_PATH_LENGTH 1024
#define MAX_FILTER_LENGTH 256







void check_sf_format(char *path)
{
    int fd = open(path, O_RDONLY);
    int version;
    u_int8_t nr_sections;
    char magic[2];
    lseek(fd, -1, SEEK_END);
    read(fd, magic, 1);
    
    magic[1] = '\0';

    if (strcmp(magic, "s") != 0)
    {
        printf("ERROR\nwrong magic\n");
        return ;
    }

    short header;

    lseek(fd, -3, SEEK_END);
    read(fd, &header, 2);

    lseek(fd, -header, SEEK_END);
    read(fd, &version,sizeof(version));
    
    if (version < 29 || version > 79)
    {
        printf("ERROR\nwrong version\n");
        return ;
    }
    read(fd, &nr_sections, sizeof(nr_sections));
    
    if (nr_sections < 8 || nr_sections > 17)
    {
        printf("ERROR\nwrong sect_nr\n");
        return ;
    }
   
	u_int32_t type = 0;
	for (int i = 0; i < nr_sections; i++)
        {
        
        lseek(fd, 20, SEEK_CUR);
        read(fd, &type, 4);
        if(type!=35 && type!=17&& type!=92 && type!=80 && type!=32){
            printf("ERROR\nwrong sect_types\n");
            close(fd);
            return;
        }
        lseek(fd, 8, SEEK_CUR);
    }
    char* name = (char*)malloc(5*sizeof(int)+1);
     int size = 0, offset = 0;
	lseek(fd, -header, SEEK_END);
        read(fd, &version, 4);
	read(fd, &nr_sections, 1);
    printf("SUCCESS\nversion=%d\nnr_sections=%d\n", version, nr_sections);
    for(int section = 0; section < nr_sections; section++){
        strcpy(name,"");
        read(fd, name, 20);
        read(fd, &type, 4);
        read(fd, &offset, 4);
        read(fd, &size, 4);
        printf("section%d: %s %d %d\n", section+1, name, type, size);
    }
    free(name);
    close(fd);
    return;
}

void list_directory(const char *dirPath, int size,const char *permissions)
{
    DIR *dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LENGTH];
    char *perm;
    perm=(char*)malloc(9);
    int k=0;

    lstat(dirPath, &inode);
   
    dir = opendir(dirPath);
    if (dir == NULL) {
        printf("ERROR\n");
        printf("invalid directory path\n");
        return;
    }
    else
    {
     printf("SUCCESS\n");
     }

    if(permissions!=NULL)
    {
        for(int i=0;i<9;i++)
        {
            if( permissions[i]=='r' || permissions[i]=='w' || permissions[i]=='x')
            {
            	
                perm[k]='1';
                k++;
            }
            if(permissions[i]=='-')
            {
                
                perm[k]='0';
                k++;
            }
        }
         //printf("%s\n",perm);
    }
    while ((dirEntry = readdir(dir)) != NULL)
    {
         if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0){

        snprintf(name, MAX_PATH_LENGTH, "%s/%s", dirPath, dirEntry->d_name);
        if(permissions!=NULL)
        {
            struct stat statbuf;
            char rights[10]={};
            if(stat(name,&statbuf)==0)
            {
                
                if(statbuf.st_mode & S_IRUSR)
                {
                    rights[0]='1';
                }
                else{
                    rights[0]='0';
                }
                if(statbuf.st_mode & S_IWUSR)
                {
                    rights[1]='1';
                }
                else{
                    rights[1]='0';
                }
                if(statbuf.st_mode & S_IXUSR)
                {
                    rights[2]='1';
                }
                 else{
                    rights[2]='0';
                }
                if(statbuf.st_mode & S_IRGRP)
                {
                    rights[3]='1';
                }
                 else{
                    rights[3]='0';
                }
                if(statbuf.st_mode & S_IWGRP)
                {
                    rights[4]='1';
                }
                 else{
                    rights[4]='0';
                }
                if(statbuf.st_mode & S_IXGRP)
                {
                    rights[5]='1';
                }
                 else{
                    rights[5]='0';
                }
                if(statbuf.st_mode & S_IROTH)
                {
                    rights[6]='1';
                }
                 else{
                    rights[6]='0';
                }
                if(statbuf.st_mode & S_IWOTH)
                {
                    rights[7]='1';
                }
                 else{
                    rights[7]='0';
                }
                if(statbuf.st_mode & S_IXOTH)
                {
                    rights[8]='1';
                }
                 else{
                    rights[8]='0';
                }
                rights[9]='\0';
                //printf("%s\n",rights);
            }
            // printf("%s\n",rights);
             if(strcmp(rights,perm)==0)
                {
                    printf("%s\n", name);
                }
        }
        else if (size >0){
            
            lstat(name, &inode);
            if (S_ISREG(inode.st_mode) && inode.st_size <size)
            {
               
                
                printf("%s\n", name);
            }
        }
  	 else{
            printf("%s\n", name);
    }
}
}

    closedir(dir);
    free(perm);
}
void list_dir_rec(char *dir_path,int first,int size,const char *permissions) {
    
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
    char *perm;
    perm=(char*)malloc(9);
    int k=0;
    

    dir = opendir(dir_path);
    if(dir == NULL)
    {
        printf("ERROR\n");
        printf("invalid directory path\n");
        return;
    }

    if(first == 0)
    {
        printf("SUCCESS\n");
        first = 1;
    }
    if(permissions!=NULL)
    {
        for(int i=0;i<9;i++)
        {
            if( permissions[i]=='r' || permissions[i]=='w' || permissions[i]=='x')
            {
            	
                perm[k]='1';
                k++;
            }
            if(permissions[i]=='-')
            {
                
                perm[k]='0';
                k++;
            }
        }
         //printf("%s\n",perm);
    }
    while((entry = readdir(dir)) != NULL)
    {
    
    	 snprintf(fullPath, 512, "%s/%s", dir_path, entry->d_name);
      

       
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
           {
            if(lstat(fullPath, &statbuf) == 0)
            {

if(permissions!=NULL)
        {
            struct stat statbuf;
            char rights[10]={};
            if(lstat(fullPath,&statbuf)==0)
            {
                
                if(statbuf.st_mode & S_IRUSR)
                {
                    rights[0]='1';
                }
                else{
                    rights[0]='0';
                }
                if(statbuf.st_mode & S_IWUSR)
                {
                    rights[1]='1';
                }
                else{
                    rights[1]='0';
                }
                if(statbuf.st_mode & S_IXUSR)
                {
                    rights[2]='1';
                }
                 else{
                    rights[2]='0';
                }
                if(statbuf.st_mode & S_IRGRP)
                {
                    rights[3]='1';
                }
                 else{
                    rights[3]='0';
                }
                if(statbuf.st_mode & S_IWGRP)
                {
                    rights[4]='1';
                }
                 else{
                    rights[4]='0';
                }
                if(statbuf.st_mode & S_IXGRP)
                {
                    rights[5]='1';
                }
                 else{
                    rights[5]='0';
                }
                if(statbuf.st_mode & S_IROTH)
                {
                    rights[6]='1';
                }
                 else{
                    rights[6]='0';
                }
                if(statbuf.st_mode & S_IWOTH)
                {
                    rights[7]='1';
                }
                 else{
                    rights[7]='0';
                }
                if(statbuf.st_mode & S_IXOTH)
                {
                    rights[8]='1';
                }
                 else{
                    rights[8]='0';
                }
                rights[9]='\0';
                //printf("%s\n",rights);
            }
            // printf("%s\n",rights);
             if(strcmp(rights,perm)==0)
                {
                    printf("%s\n", fullPath);
                }
        }

 		    else if(size>0)
      	     {
            	lstat(fullPath, &statbuf);
            	if (S_ISREG(statbuf.st_mode) && statbuf.st_size <size)
            	{
                		printf("%s\n", fullPath);
            	}
        	    }
		    else{
			 printf("%s\n", fullPath);
			}
                if(S_ISDIR(statbuf.st_mode))
                {
                    first = 1;
                    list_dir_rec(fullPath,first,size,permissions);
                }
            }	
        }
    }
    closedir(dir);

}
   
int main(int argc, char **argv){
    char *path;
    int first=0;
    int size=0;
    char *permissions=NULL;
    
    if(argc >= 2)
    {
        if(strcmp(argv[1], "variant") == 0)
        {
            printf("46940\n"); 
        }
        if(strcmp(argv[1], "list") == 0)
        {
           if(argc ==3)
           {
           	
          	path=strtok(argv[2],"=");
          	path=strtok(NULL,"=");
          	list_directory(path,size,permissions);	
       
    	   }
    	   if(argc>=3)
    	   {
    	       if(argc==4)
                {
                    if(strcmp(argv[2], "recursive") == 0)
                    {
                        if(strncmp(argv[3], "path=",5) == 0)
                        {
                            path=strtok(argv[3],"=");
          	                path=strtok(NULL,"=");
          	                list_dir_rec(path,first,size,permissions);
                        }
                    }
                    if(strncmp(argv[2], "path=",5) == 0)
                    {
                        path=strtok(argv[2],"=");
          	            path=strtok(NULL,"=");
          	            if(strcmp(argv[3], "recursive") == 0)
          	            {
                           
          	                list_dir_rec(path,first,size,permissions);
          	            }
                        if(strncmp(argv[3], "size_smaller=",13) == 0)
          	            {
                            size=atoi(argv[3]+13);
                           
          	            }
                        if(strncmp(argv[3],"permissions=",12)==0)
                        {
                            permissions=strtok(argv[3],"=");
                            permissions=strtok(NULL,"=");
                        }
                        
                        list_directory(path,size,permissions);
                    }
                    if(strncmp(argv[2], "size_smaller=",13) == 0)
                    {
                       size=atoi(argv[2]+13);
                       //printf("%d\n",size);
                       if(strncmp(argv[3], "path=",5) == 0)
                       {
                         path=strtok(argv[3],"=");
                         path=strtok(NULL,"=");
                         list_directory(path,size,permissions);
                       }
                    }
                    if(strncmp(argv[2],"permissions=",12)==0)
                    {
                        permissions=strtok(argv[2],"=");
                        permissions=strtok(NULL,"=");
                        //printf("%s\n",permissions);
                        if(strncmp(argv[3], "path=",5) == 0)
                        {
                            path=strtok(argv[3],"=");
          	                path=strtok(NULL,"=");
                             
                            list_directory(path,size,permissions);
                        }
                    }
                   
                 
                    
                }
                if(argc>=4)
                {
                  if(argc==5)
                  {
                    if(strcmp(argv[2], "recursive") == 0) 
                    {
                       if(strncmp(argv[3], "size_smaller=",13) == 0)
                    	{
                          size=atoi(argv[3]+13);
                           if(strncmp(argv[4], "path=",5) == 0)
                           {
                               path=strtok(argv[4],"=");
          	               path=strtok(NULL,"=");
          	             
                           }
                        }
                       if(strncmp(argv[3],"permissions=",12)==0)
                       {
                            permissions=strtok(argv[3],"=");
                            permissions=strtok(NULL,"=");
                            if(strncmp(argv[4], "path=",5) == 0)
                           {
                               path=strtok(argv[4],"=");
          	               path=strtok(NULL,"=");
          	               
                           }
                        }
                        if(strncmp(argv[3], "path=",5) == 0)
                        {
                            path=strtok(argv[3],"=");
          	               path=strtok(NULL,"=");
          	               if(strncmp(argv[4], "size_smaller=",13) == 0)
		            	    {
		                        size=atoi(argv[4]+13);
		                   	
		                    }
		                    if(strncmp(argv[4],"permissions=",12)==0)
                       		    {
		                    permissions=strtok(argv[4],"=");
		                    permissions=strtok(NULL,"=");
		                   
                            	    } 
		                	
                        }
                      
                      list_dir_rec(path,first,size,permissions);

                    }
             if(strncmp(argv[2], "path=",5) == 0)
                  {
                    path=strtok(argv[2],"=");
          	        path=strtok(NULL,"=");
          	        if(strncmp(argv[3], "size_smaller=",13) == 0)
		          {
		             size=atoi(argv[3]+13);
                        if(strcmp(argv[4], "recursive") == 0) 
                        {
                           list_dir_rec(path,first,size,permissions);  
                        }  
		          }
                  
		           if(strncmp(argv[3],"permissions=",12)==0)
                   	   {
		             permissions=strtok(argv[3],"=");
		             permissions=strtok(NULL,"=");
		               
		              if(strcmp(argv[4], "recursive") == 0) 
		                {
		                    list_dir_rec(path,first,size,permissions);
		                } 
                        
		            }
                    if(strcmp(argv[3], "recursive") == 0)  
                    {
                        if(strncmp(argv[4],"permissions=",12)==0)
                        {
                           permissions=strtok(argv[4],"=");
		           permissions=strtok(NULL,"="); 
                           
                        }
                        if(strncmp(argv[4], "size_smaller=",13) == 0)
                        {
                            size=atoi(argv[4]+13);
                        }
			
                        list_dir_rec(path,first,size,permissions);
                    }
                    
                  }
    	   	
    	        
    	        if(strncmp(argv[2], "size_smaller=",13) == 0)
    	        {
                    
    	            size=atoi(argv[2]+13);
    	            if(strcmp(argv[3], "recursive") == 0)
		      {        
		        if(strncmp(argv[4], "path=",5) == 0)
                  	  {
                              path=strtok(argv[4],"=");
          	              path=strtok(NULL,"=");
          	               
          	              list_dir_rec(path,first,size,permissions);
          	            }
    	           }
    	           if(strncmp(argv[3], "path=",5) == 0)
                   {
                      path=strtok(argv[3],"=");
                      path=strtok(NULL,"=");
                      if(strcmp(argv[4], "recursive") == 0)
          	            {
          	              
                            list_dir_rec(path,first,size,permissions);
          	            }
                        
                   }
                  
                    
    	        }
                if(strncmp(argv[2],"permissions=",12)==0)
                {
                    permissions=strtok(argv[2],"=");
		    permissions=strtok(NULL,"=");
                     if(strcmp(argv[3], "recursive") == 0)
    	            {
    	               
                       if(strncmp(argv[4], "path=",5) == 0)
                        {
                            path=strtok(argv[4],"=");
                            path=strtok(NULL,"=");
                     	  
                            list_dir_rec(path,first,size,permissions);
                        }
                    }
                    if(strncmp(argv[3], "path=",5) == 0)
                    {
                       path=strtok(argv[3],"=");
                        path=strtok(NULL,"=");
                        if(strcmp(argv[4], "recursive") == 0)
          	            {
          	            
                            list_dir_rec(path,first,size,permissions);
          	            }

                    } 
                    
                     
                }
            }
        }
    }
}
if(argc==3)
{
   if(strcmp(argv[1],"parse")==0)
   {
      if(strncmp(argv[2], "path=",5) == 0)
      {
      path=strtok(argv[2],"=");
      path=strtok(NULL,"=");
      check_sf_format(path);
     }
   }
   if(strcmp(argv[2],"parse")==0)
   {
      if(strncmp(argv[1], "path=",5) == 0)
      {
      path=strtok(argv[1],"=");
      path=strtok(NULL,"=");
      check_sf_format(path);
     }
   }
   	
}
}
                

return 0;
}

