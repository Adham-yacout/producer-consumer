#include<stdio.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/shm.h>
#include<stdlib.h>
#include <iostream>
#include <chrono>
#include <random>
#include<string>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<errno.h>
#include<unistd.h>
#include<cstdlib>
#include<ctime>
#include<time.h>
#include<sys/time.h>

struct memory{
char name[15];
double price=00.00;

};
using namespace std;
//mutex_sem =s buffer_sum=n full_sem =size of buffer
int mutex_sem,buffer_sem,full_sem;

int main(int argc, char**argv)
{	

int *i;

	key_t keyi = ftok("indexfile.txt",20000);
	 
	 int shmidi = shmget(keyi,sizeof(int),0666);
	 i=(int*)shmat(shmidi,0,0);
	
	char producername[50];
        strcpy(producername,argv[1]);
        double  mean = stod(argv[2]);
        double sd =stod(argv[3]);
        int sleep1=stoi(argv[4]);
        int n=stoi(argv[5]);
        key_t keys =ftok ("sem.txt", 1200);
	int semids = semget(keys,1,0666);
	int e= semctl (semids,0,SETVAL, n);
	
	int memoryshid;
	key_t key=ftok("memory",65);
	key_t s_key=ftok("semakey",400);//semaphores key
	mutex_sem=semget(s_key,1,0666 ); //hycreate el sema mutex 
	semctl(mutex_sem,1,SETVAL,1);//setval->max value for semaphore is 1 and intialize sem to 1
	
	buffer_sem=semget(s_key,1,0666 ); //hyst2bl el n sema
	semctl(buffer_sem,1,SETVAL,0);
	full_sem=semget(s_key,1,0666); //hycreate el fullsema
	semctl(full_sem,1,SETVAL,n);
	
	struct memory * sharedmemory;//constructing shared memory
	memoryshid=shmget(key,sizeof(struct memory)*n, 0666 | IPC_CREAT);
	if(memoryshid<0)
	{
	exit(1);
	}
	sharedmemory= (struct memory *)shmat(memoryshid,NULL,0);

	  

	
	struct sembuf wait={0,-1,IPC_NOWAIT};//for wait signals
	struct sembuf signal={0,1,IPC_NOWAIT};//for wait signals
	double distr;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	  std::default_random_engine generator (seed);
	  std::normal_distribution<double> distribution (mean,sd);
	while(1)
	{
	char buff[100];
	time_t tim = time(NULL);
  	struct tm tm = *localtime(&tim);
  	cout<<""<<endl;
  	timeval getmilli;
  	gettimeofday(&getmilli,NULL);
  	int timemilli=getmilli.tv_usec/1000;
  	//time_t is a struct that starts counting of years from 1900
  	dup(2);
  	printf("\033[;31m[%02d/%02d/%d %02d:%02d:%02d.%03d] \033[0m", tm.tm_mon + 1, tm.tm_mday,tm.tm_year+1900, tm.tm_hour, tm.tm_min,tm.tm_sec,timemilli );
  	dup(0);
	 //critical section
	 //producing
	  
	  //produce
	  distr=distribution(generator);
	 dup(2);
	  printf("\033[;31m%s: generating a new value %.2lf\033[0m",producername,distr);
	   cout<<""<<endl;
	   
	   printf("\033[;31m[%02d/%02d/%d %02d:%02d:%02d.%03d] \033[0m", tm.tm_mon + 1, tm.tm_mday,tm.tm_year+1900, tm.tm_hour, tm.tm_min,tm.tm_sec,timemilli );
	  printf("\033[;31m%s: trying to get mutex on shared buffer\033[0m",producername);
	   cout<<""<<endl;	
	   dup(0); 
	  semop(full_sem,&wait,1);//1 indicates size of array =1
	  
	  semop(mutex_sem,&wait,1);//1 indicates size of array =1
	  //append
	  dup(2);
	 printf("\033[;31m[%02d/%02d/%d %02d:%02d:%02d.%03d] \033[0m", tm.tm_mon + 1, tm.tm_mday,tm.tm_year+1900, tm.tm_hour, tm.tm_min,tm.tm_sec,timemilli );
	  printf("\033[;31m%s: placing %.2lf on shared buffer \033[0m",producername,distr);
	   cout<<""<<endl;
	   dup(0);
	  if(*i<n)
	  { dup(2);
	    printf("\033[;31m[%02d/%02d/%d %02d:%02d:%02d.%03d] \033[0m", tm.tm_mon + 1, tm.tm_mday,tm.tm_year+1900, tm.tm_hour, tm.tm_min,tm.tm_sec,timemilli );
	   printf("\033[;31m%s: sleeping for %d ms\033[0m",producername,sleep1); 
	   dup(0);
	  sharedmemory[*i].price=distr;
	  strcpy(sharedmemory[*i].name,producername);
	  *i+=1;
	  //sharedmemory[].counter=;
	
	  sleep(sleep1);
	  }
	  else{
	  dup(2);
	 printf("\033[;31m[%02d/%02d/%d %02d:%02d:%02d.%03d] \033[0m", tm.tm_mon + 1, tm.tm_mday,tm.tm_year+1900, tm.tm_hour, tm.tm_min,tm.tm_sec,timemilli );
	  printf("\033[;31m%s: sleeping for %d ms\033[0m",producername,sleep1);
	  dup(0);
	  *i=0;
	  sharedmemory[*i].price=distr;
	  strcpy(sharedmemory[*i].name,producername);
	  *i+=1;
	 
	   
	  sleep(sleep1);
	  }
	 cout<<""<<endl;
	  semop(mutex_sem,&signal,1);//1 indicates size of array =1
	  semop(buffer_sem,&signal,1);//1 indicates size of array =1
	
	
	}
	shmdt(sharedmemory);
shmdt(i); 
return 0;
}      
