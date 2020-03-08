/*
 * HelloThread.c -Pthreads "hello, world" program
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
	int self_id;
	int item_count;
} Thread;

void*producer(void*vargp);
void*consumer(void*amount);

void createProducerThreads(int amount, int item_count)
{
	pthread_t * array = malloc(amount* sizeof(pthread_t));
	int count = 0;
	while(count < amount)
	{
		Thread week = {.item_count = item_count, .self_id = count};
		void* dummy = &week;
		pthread_create(&array[count],NULL,producer,dummy);
		pthread_join(array[count], NULL);
		count++;
	}
}

void createConsumerThreads(int amount, int item_count)
{
	pthread_t * array = malloc(amount* sizeof(pthread_t));
	int count = 0;
	while(count < amount)
	{
		Thread week = {.item_count = item_count, .self_id = count};
		void* dummy = &week;
		pthread_create(&array[count],NULL,consumer,dummy);
		pthread_join(array[count], NULL);
		count++;
	}
}

int main()
{
	createProducerThreads(10, 10);
	createConsumerThreads(10, 100);
	// pthread_t * array = malloc(10* sizeof(pthread_t));
	// int count = 0;
	// while( count < 10)
	// {
	// 	// printf("count: %d\n", count);
	// 	Thread week = {.item_count = 10, .self_id = count};
	// 	void* dummy = &week;
	// 	pthread_create(&array[count],NULL,thread,dummy);
	// 	pthread_join(array[count], NULL);
	// 	count++;
	// }
	// single thread intialization for 
	// pthread_t tid;
	// pthread_create(&tid,NULL,thread,ptr);
	// pthread_join(tid,NULL);

	// int amount = 5;
	// int size = 10;
	// void *ptr = &size;
	// pthread_t tid, answer;
	// pthread_create(&tid,NULL,thread,NULL);
	// pthread_join(tid,NULL);
	// pthread_create(&answer,NULL,producer,ptr);
	// pthread_join(answer,NULL);
	// printf("Hello from after thr producer.\n");

	// pthread_t * array = malloc( 2 * sizeof(pthread_t));
	// int count = 0;
	// // ptr = &size;
	// while(count <  2)
	// {
		
	// 	pthread_create(&array[count], NULL, producer, ptr);
	// 	pthread_join(array[count], NULL);
	// 	count++;
	// }
	

	exit(0);
}

void *producer(void *vargp) /* thread routine */
{
	printf("Inside Producer Function.\n");
	// pthread_detach(pthread_self());
	int item_count = (*((Thread*)vargp)).item_count;
	int self_id =    (*((Thread*)vargp)).self_id;
	printf("Self ID: %d, item_count: %d.\n", self_id, item_count);
	return NULL;	
}

void*consumer(void*amount)
{
	printf("Inside Consumer Function.\n");
	// pthread_detach(pthread_self());
	int item_count = (*((Thread*)amount)).item_count;
	int self_id =    (*((Thread*)amount)).self_id;
	printf("Self ID: %d, item_count: %d.\n", self_id, item_count);
	return NULL;
}
