//Name: Philip Huang ID: 17535384
//Name: Carlos Yan Ho ID:56051179
/*
 * HelloThread.c -Pthreads "hello, world" program
 */

#include <stdio.h>
#include <unistd.h>
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
void*producerSleep(void*vargp);
void*consumerSleep(void*amount);


// Wrapper functions that creates groups of threads
void createProducerThreads(int amount, int item_count, int delay);
void createConsumerThreads(int amount, int item_count, int delay);

sbuf_t buffer;



pthread_t * producerArray;
pthread_t * consumerArray;

int main(int argc, char ** argv)

{
	if (argc != 5){
		printf("Invalid amount of arguments\n");
		exit(0);
	}
	// int c = 0;
	// while (c < argc){
	// 	printf("arguments [%d]: %s \n",c, argv[c]);
	// 	c++;
	// }

	// int p = atoi(argv[1]);
	// int c = atoi(argv[2]);
	// int i = atoi(argv[3]);
	// int d = atoi(argv[4]);
	int p = atoi(argv[1]);
	int c = atoi(argv[2]);
	int i = atoi(argv[3]);
	int d = atoi(argv[4]);


	//p between 1-16

	if ( between1to16(p) && i ){		//checks p and i
		// printf("p:%d , between: %d \n", p, between1to16(p) );
		if ( between1to16(c) && (c < c*i) ){		//checks c and c < p*i
			// printf("arguments are correct\n" );
		}
	 }
	//c between 1-16 and less than the total items produced (c < p*i)

	//i items produced by each producer

	//d selection of delay option. if 0, delay added to producer; if 1 delay added to consumers



	sbuf_init(&buffer,8);
	// printf ("test \n");
	createProducerThreads(p, i, d);
	createConsumerThreads(c, p * i, d);
	// printf ("test \n");
	int count = 0;
	while(count < p)
	{
		pthread_join(producerArray[count], NULL);
		//printf("waiting for producer %d\n", count);
		count++;
	}
	count = 0;
	while(count < c)
	{
		pthread_join(consumerArray[count], NULL);
		// printf("waiting for consumer %d\n", count);
		count++;
	}


	sbuf_deinit(&buffer);
	exit(0);
}

int between1to16(int num){
	if (num >= 1 && num <= 16){
		return 1;
	}
	return 0;
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
	int item_count = (*((Thread*)vargp)).item_count;
	int self_id =    (*((Thread*)vargp)).self_id;
	int count = (self_id * item_count);
	while(count <= (self_id + 1) * item_count - 1){
		sbuf_insert (&buffer, count);
		printf("producer_%d produced item %d \n",self_id, count);
		count++;
	}
	return NULL;	
}

void*consumer(void*amount)
{
	int item_count = (*((Thread*)amount)).item_count;
	int self_id =    (*((Thread*)amount)).self_id;
	int count = 0;
	while(count < item_count){
		printf("consumer_%d consumed item %d \n", self_id , sbuf_remove (&buffer));
		count++;	
	}
	return NULL;
}

void*producerSleep(void*vargp)
{
	int item_count = (*((Thread*)vargp)).item_count;
	int self_id =    (*((Thread*)vargp)).self_id;
	int count = (self_id * item_count);
	while(count <= (self_id + 1) * item_count - 1){
		sbuf_insert (&buffer, count);
		printf("producer_%d produced item %d \n",self_id, count);
		count++;
		usleep(500000);
	}
	return NULL;	
}

void*consumerSleep(void*amount)
{
	int item_count = (*((Thread*)amount)).item_count;
	int self_id =    (*((Thread*)amount)).self_id;
	int count = 0;
	while(count < item_count){
		printf("consumer_%d consumed item %d \n", self_id , sbuf_remove (&buffer));
		count++;
		usleep(500000);
	}
	return NULL;
}

void createProducerThreads(int amount, int item_count, int delay)
{
	// printf("inside prod thread funct\n");
	producerArray = malloc(amount* sizeof(pthread_t));
	Thread * threadArray = malloc(amount * sizeof(Thread));
	int count = 0;
	while(count < amount)
	{
		threadArray[count].item_count = item_count;
		threadArray[count].self_id = count; 
		void* dummy = &threadArray[count];
		if(delay == 0)
			pthread_create(&producerArray[count], NULL, producerSleep, dummy);
		else	
			pthread_create(&producerArray[count], NULL, producer, dummy);
		count++;
	}
}

void createConsumerThreads(int amount, int item_count, int delay)
{
	// printf("inside consumer thread funct\n");
	consumerArray = malloc(amount* sizeof(pthread_t));
	Thread * threadArray = malloc(amount * sizeof(Thread));
	int count = 0;
	while(count < amount)
	{
		threadArray[count].item_count = item_count/amount;
		threadArray[count].self_id = count; 
		void* dummy = &threadArray[count];
		if(delay == 0)
			pthread_create(&consumerArray[count], NULL, consumer, dummy);
		else
			pthread_create(&consumerArray[count], NULL, consumerSleep, dummy);
		count++;
	}
}
