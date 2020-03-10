/*
 * HelloThread.c -Pthreads "hello, world" program
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
	int self_id;
	int item_count;
} Thread;

typedef struct {
	int *buf; /* Buffer array */
	int n; /* Maximum number of slots*/
	int front; /* buf[(front+1)%n] is first item */
	int rear;  /* buf[rear%n] is last item */
	sem_t mutex; // protecxts accesses to buf
	sem_t slots; // Counts available slots
	sem_t items; // Counts available items
} sbuf_t;

// Buffer Functions
void sbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);

// Thread Functions
void*producer(void*vargp);
void*consumer(void*amount);

// Wrapper functions that creates groups of threads
void createProducerThreads(int amount, int item_count);
void createConsumerThreads(int amount, int item_count);

sbuf_t buffer;
pthread_t * producerArray;
pthread_t * consumerArray;

int main()
{

	sbuf_init(&buffer,8);
	createProducerThreads(10, 10);
	createConsumerThreads(10, 100);
	int count = 0;
	while(count < 10)
	{
		pthread_join(producerArray[count], NULL);
		// printf("waiting for producer %d\n", count);
		count++;
	}
	count = 0;
	while(count < 10)
	{
		pthread_join(consumerArray[count], NULL);
		// printf("waiting for consumer %d\n", secondCount);
		count++;
	}

	sbuf_deinit(&buffer);
	exit(0);
}

// Buffer Functions
void sbuf_init(sbuf_t *sp, int n)
{
	sp->buf = (int*)calloc(n, sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0; 	
	sem_init(&sp->mutex, 0, 1); 
	sem_init(&sp->slots, 0, n);
	sem_init(&sp->items, 0, 0); 
}

void sbuf_deinit(sbuf_t *sp)
{
	free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item)
{
	sem_wait(&sp->slots);
	sem_wait(&sp->mutex);
	sp->buf[(sp->rear)%(sp->n)] = item;
	sp->rear++;
	sem_post(&sp->mutex);
	sem_post(&sp->items);

}

int sbuf_remove(sbuf_t *sp)
{
	int item;
	sem_wait(&sp->items);
	sem_wait(&sp->mutex);
	item = sp->buf[(sp->front)%(sp->n)];
	sp->front++;
	sem_post(&sp->mutex);
	sem_post(&sp->slots);
	return item;
}



void *producer(void *vargp) /* thread routine */
{
	printf("Inside Producer Function.\n");
	// pthread_detach(pthread_self());
	// sem_wait(&mutex); 
	int item_count = (*((Thread*)vargp)).item_count;
	int self_id =    (*((Thread*)vargp)).self_id;
	printf("Self ID: %d, item_count: %d.\n", self_id, item_count);
	// sem_post(&mutex); 
	return NULL;	
}

void*consumer(void*amount)
{
	printf("Inside Consumer Function.\n");
	// sem_wait(&mutex);
	int item_count = (*((Thread*)amount)).item_count;
	int self_id =    (*((Thread*)amount)).self_id;
	printf("Self ID: %d, item_count: %d.\n", self_id, item_count);
	// sem_post(&mutex); 
	return NULL;
}

void createProducerThreads(int amount, int item_count)
{
	producerArray = malloc(amount* sizeof(pthread_t));
	Thread * threadArray = malloc(amount * sizeof(Thread));
	int count = 0;
	while(count < amount)
	{
		threadArray[count].item_count = item_count;
		threadArray[count].self_id = count; 
		void* dummy = &threadArray[count];
		pthread_create(&producerArray[count],NULL,producer,dummy);
		// pthread_join(producerArray[count], NULL);
		count++;
	}
}

void createConsumerThreads(int amount, int item_count)
{
	consumerArray = malloc(amount* sizeof(pthread_t));
	Thread * threadArray = malloc(amount * sizeof(Thread));
	int count = 0;
	while(count < amount)
	{
		threadArray[count].item_count = item_count;
		threadArray[count].self_id = count; 
		void* dummy = &threadArray[count];
		pthread_create(&consumerArray[count],NULL,consumer,dummy);
		// pthread_join(consumerArray[count], NULL); // comment out to detach functions
		count++;
	}
}
