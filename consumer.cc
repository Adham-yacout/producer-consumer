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
struct data{
//inner data structure for consumer
char name[15] ;
double price=00.00;
double avgprice[5]={0,0,0,0,0};
double avergprice=00.00;
int counter=0;//dah elly hy2oly ahot feen gowa el array
double quant=00.00;//dah hy2oly m3aya kam element 3lshan ahsb el average
double prevprice=00.00; //used for arrows
int arrow=3;

};
struct memory{
//shared memory
char name[15];
double price=00.00;

};
using namespace std;

void print(struct data *  sharedmemory);//used for print of output
int indexgetter(char producername[15] );//used to get index from the shared memory
int mutex_sem,buffer_sem,full_sem; //init for the used semaphores
int main(int argc, char**argv)
{

int flag=1;
struct data adham[11] ;
	strcpy(adham[0].name,"ALUMINIUM      ");
	strcpy(adham[1].name,"COPPER         ");
	strcpy(adham[2].name,"COTTON         ");
	strcpy(adham[3].name,"CRUDEOIL       ");
	strcpy(adham[4].name,"GOLD	        ");
	strcpy(adham[5].name,"LEAD	        ");
	strcpy(adham[6].name,"MENTHAOIL      ");
	strcpy(adham[7].name,"NATURALGAS     ");
	strcpy(adham[8].name,"NICKEL         ");
	strcpy(adham[9].name,"SILVER         ");
	strcpy(adham[10].name,"ZINC           ");
int tracker=0;//tracks the buffer size  
 int n=stoi(argv[1]);
int memoryshid;
	int *i;

	key_t keyi = ftok("indexfile.txt",20000);
	 
	 
	 int shmidi = shmget(keyi,sizeof(int),0666|IPC_CREAT);
	 i=(int*)shmat(shmidi,0,0);
	 *i=0;
 	
 
key_t key=ftok("memory",65);

key_t s_key=ftok("semakey",400);//semaphores key
	mutex_sem=semget(s_key,1,0666 | IPC_CREAT ); //hycreate el sema mutex 
	semctl(mutex_sem,1,SETVAL,1);//setval->max value for semaphore is 1 and intialize sem to 1
	
	buffer_sem=semget(s_key,1,0666 | IPC_CREAT); //hyst2bl el n sema
	semctl(buffer_sem,1,SETVAL,0);
	
	full_sem=semget(s_key,1,0666| IPC_CREAT); //hycreate el fullsema
	semctl(full_sem,1,SETVAL,n);
struct memory * sharedmemory;
memoryshid=shmget(key,sizeof(struct memory)*n, 0666 | IPC_CREAT);
if(memoryshid<0)
{
exit(1);
}
sharedmemory= (struct memory *)shmat(memoryshid,NULL,0);
	struct sembuf wait={0,-1,IPC_NOWAIT};//for wait signals
	struct sembuf signal={0,1,IPC_NOWAIT};//for wait signals
	char ingy[15] ;
	int index;
	printf("\e[1;1H\e[2J");
print(adham);//outpit form
while(true)
{	 
	  semop(buffer_sem,&wait,1);//1 indicates size of array =1
	  semop(mutex_sem,&wait,1);
	  if(flag)//buffer isn't filled yet
	  {
	  	while(1)
	  	{ //to always make sure that we dont access a null location
	  	if(tracker<*i)
		  	{
		  	
		  	break;
		  	}
	  	}
	  }
	  
	
	  if(tracker<n)
	  {
	  strcpy(ingy,sharedmemory[tracker].name);
	  index=indexgetter(ingy);
	
	 adham[index].prevprice=adham[index].price;//previous price
	  
	  adham[index].price=sharedmemory[tracker].price;//update price
	  
	  adham[index].avgprice[adham[index].counter]=adham[index].price;
	  adham[index].counter+=1;
	  tracker+=1;
	  adham[index].quant+=1;
	  
	  }
	  else{
	  tracker=0;
	  strcpy(ingy,sharedmemory[tracker].name);
	  index=indexgetter(ingy);
	  
	  adham[index].prevprice=adham[index].price;
	 
	  adham[index].price=sharedmemory[tracker].price;
	   adham[index].avgprice[adham[index].counter]=adham[index].price;
	  adham[index].counter+=1;
	 // adham[index].counter+=1;
	  adham[index].quant+=1;
	  tracker+=1;
	  }
	  
	  if( adham[index].prevprice>adham[index].price)
	  {
	  adham[index].arrow=1;
	  }
	  else{
	  adham[index].arrow=2;
	  }
	  
	 //used to get avgprice 
	 if(adham[index].counter>4)
	  {
	    adham[index].counter=0;
	  }
	  if(adham[index].quant>5)
	  {
	    adham[index].quant=5;
	  }
	  int k;
	  double avgtemp=0;
	  for(k=0;k<5;k++)
	  {
	  avgtemp=avgtemp+adham[index].avgprice[k];
	  }
	    adham[index].avergprice=(avgtemp/adham[index].quant);
	  
	    semop(mutex_sem,&signal,1);//1 indicates size of array =1
	    semop(full_sem,&signal,1);//1 indicates size of array =1
	
	  printf("\e[1;1H\e[2J");
	  
	    print(adham);
	    
	    sleep(5);
}

shmdt(sharedmemory);
shmdt(i);
return 0;
}
int indexgetter(char producername [15] )
{   int index;
	if(!strcasecmp(producername,"ALUMINIUM"))
	{
		index=0;
	}
	else if(!strcasecmp(producername,"COPPER"))
	{
	index=1;
	}
	else if(!strcasecmp(producername,"COTTON"))
	{
	index=2;
	}
	else if(!strcasecmp(producername,"CRUDEOIL"))
	{
	index=3;
	}
	else if(!strcasecmp(producername,"GOLD"))
	{
	index=4;
	}
	else if(!strcasecmp(producername,"LEAD"))
	{
	index=5;
	}
	else if(!strcasecmp(producername,"MENTHAOIL"))
	{
	index=6;
	}
	else if(!strcasecmp(producername,"NATURALGAS"))
	{
	index=7;
	}
	else if(!strcasecmp(producername,"NICKEL"))
	{
	index=8;
	}
	else if(!strcasecmp(producername,"SILVER"))
	{
	index=9;
	}
	else if(!strcasecmp(producername,"ZINC"))
	{
	index=10;
	}
	

	return index;
}
void print(struct data *  sharedmemory){

printf("+ ");
int i=0;
for(i=0;i<38;i++)
{
	printf("-");
 }
printf("+");
printf("\n");
printf("  CURRENCY");
printf("    |Price    ");
printf("    |AvgPrice   ");
printf("|");
printf("\n");
printf("+ ");
for(i=0;i<38;i++)
{
	printf("-");
 }
 printf("+ ");
 printf("\n");
 
for(i=0;i<11;i++)
{       
printf("|%s|",sharedmemory[i].name);
       if(sharedmemory[i].arrow==2)
{
 
	printf("\033[;32m  %7.2lf ↑ \033[0m",sharedmemory[i].price);
	printf("|");
	printf("\033[;32m%7.2lf ↑ \033[0m",sharedmemory[i].avergprice);
	printf("|");
}
else if(sharedmemory[i].arrow==1){
	printf("\033[;31m  %7.2lf ↓ \033[0m",sharedmemory[i].price);
	printf("|");
	printf("\033[;31m%7.2lf ↓ \033[0m",sharedmemory[i].avergprice);
	printf("|");
}
else{
	printf("  %7.2lf   |",sharedmemory[i].price);
	printf("%7.2lf   |",sharedmemory[i].avergprice);
}
	printf("\n");
}

printf("+ ");

for(i=0;i<38;i++)
{
	printf("-");
 }
printf("+");
 printf("\n");
 
}
