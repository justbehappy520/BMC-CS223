#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define N 10 // number of elements

// global variables
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int num_items = 0;
int in = 0;
int out = 0;
int buff[N];

// circular queue?


// producer function
int produce_item() {
	return rand() % 10;
}

void *producer(void *arg) {
    int item;
    while (1) {
        item = produce_item();
        sleep(1);
        pthread_mutex_lock(&mutex);
        while (num_items >= N) {
            printf("Buffer FULL! please wait~\n");
            pthread_cond_wait(&full, &mutex);
        }
        buff[in] = item;
        in = (in + 1) % N;
        num_items++;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// consumer function
void consume_item(int item) {
	printf("Received item: %d\n", item);
}

void *consumer(void *arg) {
    int item;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (num_items == 0) {
            pthread_cond_wait(&empty, &mutex);
        }
        item = buff[out];
        out = (out + 1) % N;
        num_items--;
        pthread_mutex_unlock(&mutex);
        consume_item(item);
    }
    return NULL;
}

int main()
{
	pthread_t producer_thread, consumer_thread;
	srand(time(NULL));
	if (pthread_create(&producer_thread, NULL, producer, NULL) != 0) {
        	printf("producer thread died :(");
        	return 1;
    	}

   	if (pthread_create(&consumer_thread, NULL, consumer, NULL) != 0) {
        	printf("consumer thread died :(");
        	return 1;
    	}

    	pthread_join(producer_thread, NULL);
    	pthread_join(consumer_thread, NULL);
	return 0;
}
