#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define FIFO_WRITE "RESP_PIPE_46940"
#define FIFO_READ "REQ_PIPE_46940"
int dim=5;
int nr_echo=4;
int nr_var=7;
int nr_create_shm=10;
int nr_error=5;
int nr_success=7;
int nr_write_to_shm=12;
unsigned int val = 46940;
int nr_map_file=8;
int nr_read_from_file_offset=21;

int main()
{
    int fd_read = -1;
    int fd_write = -1;	
    char* data = NULL;
    int fd = -1;
    int shm= -1;
    char* pointerFile = NULL;
    char* pointerMem = NULL;
    unsigned int mem_size = 0;
    unsigned int nrChar = 0;
    off_t sizeFile = 0;
    char* file_name = NULL;
    unsigned int a = 0;   
    unsigned offset = 0;
    unsigned value = 0;
    int nr;
    
     
    
    
    if(mkfifo(FIFO_WRITE,0600) != 0)
    {
        printf("ERROR\ncannot create the response pipe\n");
        return 1;
    }

    

    fd_read = open(FIFO_READ,O_RDONLY);
    if(fd_read == -1)
    {
        printf("ERROR\ncannot open the request pipe\n");
        unlink(FIFO_WRITE);
        return 1;
    }

    fd_write = open(FIFO_WRITE,O_WRONLY);

    if(fd_write == -1)
    {
        printf("ERROR\ncannot open the response pipe\n");
        close(fd_read);
        unlink(FIFO_WRITE);
        return 1;
    }
    

    if((write(fd_write,&dim,1) != 1) || (write(fd_write,"HELLO",5) != 5))
    {
        printf("ERROR\n");
        return 1;
    }
    printf("SUCCESS\n");
   

    


    while(1)
    {
        
        read(fd_read,&a,1);
        data=(char*)malloc(a * sizeof(char));
        read(fd_read,data,a);
        
        if(strcmp(data,"ECHO") == 0)
        {
            
            write(fd_write,&nr_echo,1);
            write(fd_write,"ECHO",4);
            
            write(fd_write,&val,4);
            write(fd_write,&nr_var,1);
            write(fd_write,"VARIANT",7);
            
        }
        if(strcmp(data,"CREATE_SHM") == 0)
        {
            read(fd_read,&mem_size,sizeof(mem_size));
           

            shm = shm_open("/1lhC4mV",O_CREAT | O_RDWR,0664);

            if(shm < 0)
            {
               
                write(fd_write,&nr_create_shm,1);
                write(fd_write,"CREATE_SHM",10);
               
                write(fd_write,&nr_error,1);
                write(fd_write,"ERROR",5);
                return 0;

            }
            ftruncate(shm,mem_size);
            pointerMem=(char*)mmap(0,mem_size,PROT_READ | PROT_WRITE,MAP_SHARED,shm,0);

            if(pointerMem == (void*)-1)
            {
               
                write(fd_write,&nr_create_shm,1);
                write(fd_write,"CREATE_SHM",10);
                write(fd_write,&nr_error,1);
                write(fd_write,"ERROR",5);
                return 0;

            }
           
            write(fd_write,&nr_create_shm,1);
            write(fd_write,"CREATE_SHM",10);
            write(fd_write,&nr_success,1);
            write(fd_write,"SUCCESS",7);

        }
        if(strcmp(data,"WRITE_TO_SHM") == 0)
        {

           
           

            read(fd_read,&offset,sizeof(offset));
            read(fd_read,&value,sizeof(value));

            if(offset<0 || offset+3>1398620)
            {
                
                write(fd_write,&nr_write_to_shm,1);
                write(fd_write,"WRITE_TO_SHM",12);
       
                write(fd_write,&nr_error,1);
                write(fd_write,"ERROR",5);
                return 0;

            }
           
            memcpy(&pointerMem[offset],(void*)&value,4);
       
            write(fd_write,&nr_write_to_shm,1);
            write(fd_write,"WRITE_TO_SHM",12);
            write(fd_write,&nr_success,1);
            write(fd_write,"SUCCESS",7);
        }
        if(strcmp(data,"MAP_FILE") == 0)
        {
            nr = 0;
            read(fd_read,&nrChar,1);
            file_name = (char*)malloc(nr*sizeof(char));
            read(fd_read,file_name,nrChar);
            
            fd=open(file_name,O_RDONLY);
            if(fd == -1)
            {
                
                write(fd_write,&nr_map_file,1);
                write(fd_write,"MAP_FILE",8);
                write(fd_write,&nr_error,1);
                write(fd_write,"ERROR",5);
               
		return 0;

            }
            sizeFile=lseek(fd,0,SEEK_END);
            lseek(fd,0,SEEK_SET);

            pointerFile=(char*)mmap(NULL,sizeFile,PROT_READ,MAP_PRIVATE,fd,0);
            if(pointerFile == (void*)-1)
            {
               
                write(fd_write,&nr_map_file,1);
                write(fd_write,"MAP_FILE",8);
                
                write(fd_write,&nr_error,1);
                write(fd_write,"ERROR",5);
                
            }
           
            write(fd_write,&nr_map_file,1);
            write(fd_write,"MAP_FILE",8);
            
            write(fd_write,&nr_success,1);
            write(fd_write,"SUCCESS",7);
            

        }
        
        
        
        if(strcmp(data,"EXIT") == 0)
        {
            close(fd_read);
            close(fd_write);
            munmap(pointerMem,sizeof(char)*1398620);
            munmap(pointerFile,sizeof(char)*sizeFile);
            pointerMem = NULL;
            close(shm);
            
            
            unlink(FIFO_WRITE);
            return 0;
        }
        

    return 0;
}
}
