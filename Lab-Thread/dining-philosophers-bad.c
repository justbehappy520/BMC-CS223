#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define N 5
#define ROUNDS 100

pthread_mutex_t mutex[N];
struct thread_data {
	int id;
};

void *Philosophize1(void *args) {
  struct thread_data* data = (struct thread_data*) args;

  printf("Starting thread %d\n", data->id);
  for (int i = 0; i < ROUNDS; i++) {
    int decision = rand() % 2;
    if (decision == 0) {
      printf("%d is thinking\n", data->id);
      sleep(1);
    }
    else {
      printf("%d is hungry\n", data->id);

      int leftFork = data->id;
      int rightFork = (data->id + 1) % N;

      pthread_mutex_lock(&mutex[leftFork]);
      pthread_mutex_lock(&mutex[rightFork]);
      printf("%d is eating\n", data->id);
      sleep(i);
      pthread_mutex_unlock(&mutex[leftFork]);
      pthread_mutex_unlock(&mutex[rightFork]);
    }
  }
  return NULL;
}

int main()
{
  struct thread_data data[N];
  for (int i = 0; i < N; i++) {
    pthread_mutex_init(&(mutex[i]), NULL);
    data[i].id = i;
  }

  pthread_t threads[N];

  for (int i = 0; i < N; i++) {
    pthread_create(&threads[i], NULL, Philosophize1, &(data[i]));
  }

  for (int i = 0; i < N; i++) {
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < N; i++) {
    pthread_mutex_destroy(&(mutex[i]));
  }
  return 0;
}
