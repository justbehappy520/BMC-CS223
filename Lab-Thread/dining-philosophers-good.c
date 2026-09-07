#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define N 5
#define ROUNDS 100

pthread_mutex_t mutex[N];
struct thread_data {
  int id;
};

void *Philosophize2(void *args) {
  struct thread_data* data = (struct thread_data*) args;
  
  printf("Starting thread %d\n", data->id);
  for (int i = 0; i < ROUNDS; i++) {
    float duration = (rand() % 5)/5.0 + 1.0; // do something for [1,6] seconds

    int decision = rand() % 2;
    if (decision == 0) {
      printf("%d is thinking\n", data->id);
      sleep(duration);
    }
    else {
      printf("%d is hungry\n", data->id);

      int fork1 = data->id; // left
      int fork2 = (data->id + 1) % N; // right

      if (fork1 > fork2) { // for the last philosopher
        int tmp = fork2;
        fork2 = fork1;
        fork1 = tmp;
      }

      pthread_mutex_lock(&mutex[fork1]); 
      pthread_mutex_lock(&mutex[fork2]);
      printf("%d is eating\n", data->id);
      sleep(duration);
      pthread_mutex_unlock(&mutex[fork2]);
      pthread_mutex_unlock(&mutex[fork1]);
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
    pthread_create(&threads[i], NULL, Philosophize2, &(data[i]));
  }

  for (int i = 0; i < N; i++) {
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < N; i++) {
    pthread_mutex_destroy(&(mutex[i]));
  }
  return 0;
}
