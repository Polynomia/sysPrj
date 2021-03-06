/*
  This program is to solve burger buddies problem, using three threads:
  cook, cashier, customer;
  and five semaphores: rack,burger,order,server,getBurger:
   * Rack's initial value equals to the racksize. Cooks only can make 
     burgurs when rack is bigger than 0.
   * Burgur's initial value is 0. When cooks make a burgur, it adds 1.
   * Order is initialized to 0. When a customer comes, he waits for 
     sever, then order adds 1.
   * Server's initial value equals to the number of cashiers. If server
     is bigger than 0, a cashier will accept the order.
   * getBuerger is initialized to be 0. If there are burgers, the order
     will be handled, getBurger adds 1, customer leaves. Order and burger 
     sub 1.
*/

#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

sem_t rack,burger,order,server,getBurger;


void *cook(void *num){
	int i=*(int*)num;
	while(1){
		sem_wait(&rack);
		sem_post(&burger);
		printf("%s%d%s","Cook [",i,"] makes a burger.\n");
		usleep(rand()%400);   //if there is no sleep, this thread will run many times.
	}	
}
//i think cashiers don't need to sleep, they should serve all the time.

void *cashier(void *num)
{
	int i=*(int *)num;
	while(1){
		sem_wait(&order);
		printf("%s%d%s","Cashier [",i,"] accepts an order.\n");  
		sem_wait(&burger);
		sem_post(&getBurger);
		printf("%s%d%s","Cashier [",i,"] take a burger to customer.\n");
		sem_post(&rack);
		sem_post(&server);
	}
}


void *customer(void *num)
{
	int i=*(int *)num;
		usleep(rand()%400);//sleep, or they will continuly come
		printf("%s%d%s","Customer [",i,"] come.\n");
		sem_wait(&server);
		sem_post(&order);
		sem_wait(&getBurger);
}

int main(int argc, char ** argv)
{
	//if there are not 4 inputs
	if(argc!=5) {
		printf("This program needs four inputs!\n");
		return 0;}

	int cooks=atoi(argv[1]);
	int cashiers=atoi(argv[2]);
	int customers=atoi(argv[3]);
	int rackSize=atoi(argv[4]);
	
	if(cooks<=0||cashiers<=0||customers<=0||rackSize<=0)
	{	
		printf("They need to be positive numbers!\n");
		return 0;
	}
	printf("Cooks [%d], Cashiers [%d], Customers [%d]\nBegin run.\n",cooks,cashiers,customers);
//	pthread_t cookpd[100],cashierpd[100],customerpd[100];
	pthread_t *cookpd,*cashierpd,*customerpd;
	cookpd=(pthread_t*)malloc(cooks*sizeof(pthread_t));
	customerpd=(pthread_t*)malloc(customers*sizeof(pthread_t));
	cashierpd=(pthread_t*)malloc(cashiers*sizeof(pthread_t));
//make sure they are allocated
	if(cookpd==NULL||customerpd==NULL||cashierpd==NULL)
		printf("allocate error!\n");
	
	sem_init(&order,0,0);
	sem_init(&server,0,cashiers);
	sem_init(&burger,0,0);
	sem_init(&rack,0,rackSize);
	sem_init(&getBurger,0,0);
	int i,*id,max;
	max=(cooks>customers)?cooks:customers;
	max=(max>cashiers)?max:cashiers;

	id=(int*)malloc(max*sizeof(int));
	for(i=0;i<max;i++)
		id[i]=i+1;
	for(i=0;i<cooks;i++){
		int *t=i+id;
		if(pthread_create(&cookpd[i],NULL,cook,t)!=0)
			printf("cook thread error!\n");
	}
	for(i=0;i<customers;i++){
		int *t=i+id;
		if(pthread_create(&customerpd[i],NULL,customer,t)!=0)      
			printf("customer thread error!\n");
	    }

	for(i=0;i<cashiers;i++){
		int *t=i+id;
		if(pthread_create(&cashierpd[i],NULL,cashier,t)!=0)
			printf("cashier thread error!\n");
	}


//	for(i=0;i<cooks;i++)
//		pthread_join(cookpd[i],NULL);
	for(i=0;i<customers;i++)
		pthread_join(customerpd[i],NULL);
//	for(i=0;i<cashiers;i++)
//		pthread_join(cashierpd[i],NULL);
	free(cookpd);
	free(customerpd);
	free(cashierpd);
	free(id);
	sem_destroy(&getBurger);
	sem_destroy(&rack);
	sem_destroy(&order);
	sem_destroy(&burger);
	sem_destroy(&server);
	return 0;	
}

