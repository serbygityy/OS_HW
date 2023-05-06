#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <signal.h>
#include <sys/sem.h>
#include <semaphore.h>

#define NR_THREADS 6
#define NR_THREADS_FIVE 7
#define NR_THREADS_FOUR 39
#define MAX_RUNNING_THREADS 4

typedef struct
{
    int id;
    int proc;
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
} TH_STRUCT;

int ok2=0;
int turn = 0;
int once = 0;
int num_running_threads=0;
sem_t semaphore4_15,semaphore5_4,semaphore1;

void *func_four(void *arg)
{
  TH_STRUCT *s = (TH_STRUCT*)arg;
  
  if(s->id==15)
  {
     sem_wait(&semaphore4_15);
  }
  sem_wait(&semaphore5_4);
  pthread_mutex_lock(s->lock);
  num_running_threads++;
  pthread_mutex_unlock(s->lock);
  if (num_running_threads >= 35 && s->id != 15)
  {
      info(BEGIN,4,s->id);
      if(num_running_threads == 37)
      {
          sem_post(&semaphore4_15);
      }	
      sem_wait(&semaphore1);
  }
  else
  {
       info(BEGIN,4,s->id);
  } 
    
   info(END,4,s->id);
   if(s->id== 15)
   {
      for(int i = 0; i<3; i++)
        {
            sem_post(&semaphore1); 
        }	
   }
   sem_post(&semaphore5_4);
   pthread_exit(0);
       
    
    
    
}




void *func_five(void *arg)
{
   TH_STRUCT *s = (TH_STRUCT*)arg;
    pthread_mutex_lock(s->lock);
     
     if(s->id!=3){
         info(BEGIN,5,s->id);
         info(END,5,s->id);
     }
     pthread_mutex_unlock(s->lock);
     return NULL;
     
     
}
void *func(void *arg)
{
    TH_STRUCT *s = (TH_STRUCT*)arg;
    pthread_mutex_lock(s->lock);
    
   
    if(s->id == 1 )
    {
    	 pthread_cond_broadcast(s->cond);
    	pthread_cond_wait(s->cond, s->lock);
    }
    if(s->id !=4)
    {
        if(s->id==5){
            info(BEGIN,5,3);
            info(END,5,3);
        }
    	info(BEGIN,2,s->id);
    	turn++;
    }
    if(s->id == 4)
    {
       info(BEGIN,2,s->id);
       turn++;
       ok2=1;
       pthread_cond_broadcast(s->cond);
       pthread_cond_wait(s->cond, s->lock);
          
    }
     if( ok2==1)
     {
     	 pthread_cond_broadcast(s->cond);
     	 
     }
    
   if(s->id!=4 )
    {
       info(END,2,s->id);  
    }
    if(turn==5&&once == 0){
    once++;
    info(END,2,4); 
    }
   
   
   
    pthread_mutex_unlock(s->lock);

    return NULL;
}

int main(){
    init();
    sem_init(&semaphore4_15,0,0);
    sem_init(&semaphore5_4,0,4);
    sem_init(&semaphore1,0,0);	
    pid_t pid2=-1, pid3=-1, pid4=-1, pid5=-1, pid6=-1, pid7=-1, pid8=-1;	
    info(BEGIN, 1, 0);
    pid2=fork();
    waitpid(pid2,NULL,0);
    if(pid2==0)
    {
     	 info(BEGIN, 2, 0);
     	 
     	 int i;
        TH_STRUCT params[NR_THREADS];
        pthread_t tids[NR_THREADS];
        pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

         for(i=1; i<NR_THREADS; i++)
           {
              params[i].id = i;
              params[i].proc = 2;
              params[i].lock = &lock;
              params[i].cond = &cond;
              pthread_create(&tids[i], NULL, func, &params[i]);

           }
        for(i=1; i<NR_THREADS; i++)
        {
                        
           pthread_join(tids[i], NULL);
        }
        
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&lock);
     	  pid5 = fork();
     	  waitpid(pid5,NULL,0);
     	  if(pid5 == 0)
           {
             info(BEGIN, 5, 0);
	     int j;
              TH_STRUCT params_five[NR_THREADS_FIVE];
              pthread_t tids_five[NR_THREADS_FIVE];
              pthread_mutex_t lock_five = PTHREAD_MUTEX_INITIALIZER;
              pthread_cond_t cond_five = PTHREAD_COND_INITIALIZER;

              for(j=1; j<NR_THREADS_FIVE; j++)
               {
                      params_five[j].proc = 5;
                      params_five[j].id = j;
                      params_five[j].lock = &lock_five;
                      params_five[j].cond = &cond_five;
                      pthread_create(&tids_five[j], NULL, func_five, &params_five[j]);
                }
                        
              for(j=1; j<NR_THREADS_FIVE; j++)
              {
                  pthread_join(tids_five[j], NULL);
              }
                       
              pthread_cond_destroy(&cond_five);
              pthread_mutex_destroy(&lock_five);             
             pid8 = fork();
             waitpid(pid8, NULL, 0);
             if(pid8 == 0)
              {
                info(BEGIN, 8, 0);
                info(END, 8, 0);
                exit(0);
               }
               info(END, 5, 0); 
               exit(0);
            }
            else{
              pid6 = fork();
              waitpid(pid6,NULL,0);
               if(pid6 == 0)
           	{
           	 info(BEGIN, 6, 0); 
           	 pid7 = fork();
             	waitpid(pid7, NULL, 0);
            	 if(pid7 == 0)
              	{
                    info(BEGIN, 7, 0);
                    info(END, 7, 0);
                    exit(0);
               }
               info(END, 6, 0);
               exit(0);
           	}
             	
            }
            
           info(END, 2, 0);  
           exit(0);   	
     	  
    }
    else{
    	  
     	  pid3 = fork();
     	  waitpid(pid3,NULL,0);
     	  if(pid3==0)
     	  {
     	    info(BEGIN, 3, 0);
     	    pid4 = fork();
     	    waitpid(pid4, NULL, 0);
     	    if(pid4 == 0)
     	    {
     	       info(BEGIN, 4, 0);
 	      int k;
              TH_STRUCT params_four[NR_THREADS_FOUR];
              pthread_t tids_four[NR_THREADS_FOUR];
              pthread_mutex_t lock_four = PTHREAD_MUTEX_INITIALIZER;
              pthread_cond_t cond_four = PTHREAD_COND_INITIALIZER;

              for(k=1; k<NR_THREADS_FOUR; k++)
               {
                      params_four[k].proc = 4;
                      params_four[k].id = k;
                      params_four[k].lock = &lock_four;
                      params_four[k].cond = &cond_four;
                      pthread_create(&tids_four[k], NULL, func_four, &params_four[k]);
                }
                        
              for(k=1; k<NR_THREADS_FOUR; k++)
              {
                  pthread_join(tids_four[k], NULL);
              }
              sem_destroy(&semaphore5_4);
              sem_destroy(&semaphore4_15);
       	      sem_destroy(&semaphore1);                       
              pthread_cond_destroy(&cond_four);
              pthread_mutex_destroy(&lock_four);    	       
     	       info(END, 4, 0);
                 exit(0);
     	    }
     	     info(END, 3, 0);
     	     exit(0);
     	  }
     	 }
    
    info(END, 1, 0);
    return 0;
}
