#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 4096
#define DEFAULT_INTERVAL 3

// shared?
char buffer[BUFFER_SIZE+1]; // for the null terminator~
size_t buffer_index = 0;
pthread_mutex_t buffer_mutex;
int run_flag = 1; // control thread and main loop
int num_seconds = DEFAULT_INTERVAL;

void *logz(void *arg) {
	// control when we log in main
	while (run_flag) {
		sleep(num_seconds);
		pthread_mutex_lock(&buffer_mutex);
		if (buffer_index > 0) { // check if there are things to save
			// make a local copy of the things to save
			char local_buffer[BUFFER_SIZE+1];
			memcpy(local_buffer, buffer, buffer_index);
			local_buffer[buffer_index] = '\0';
			// clear the general buffer and reset index
			buffer_index = 0;
			pthread_mutex_unlock(&buffer_mutex);
			// track the time to make filename
			time_t current_time = time(NULL);
			char filename[128];
			snprintf(filename, sizeof(filename), ".log-%ld.txt", (long)current_time);
			// write to file
			FILE *fptr = fopen(filename, "w");
			if (!fptr) {
				printf("%s could not be written to\n", filename);
				fclose(fptr);
				pthread_exit(NULL);
			}
			fprintf(fptr, "%s", local_buffer);
                	fclose(fptr);
		} else { // nothing to save
			pthread_mutex_unlock(&buffer_mutex);
		}
	}
	pthread_exit(NULL);
}

void sigint_handler(int sig) {
	run_flag = 0;
}

int main(int argc, char* argv[]) 
{
	if (argc < 2) {
		printf("usage: %s <num_seconds>\n", argv[0]);
		return 1;
	}
	if (argc == 2) {
		num_seconds = atoi(argv[1]);
		if (num_seconds <= 0) {
			printf("invalid number of seconds, using default: %d\n", DEFAULT_INTERVAL);
			num_seconds = DEFAULT_INTERVAL;
		}
	}

	// mutex and signal
	if (pthread_mutex_init(&buffer_mutex, NULL) != 0) {
		printf("something died: buffer mutex probably\n");
		return 1;
	}
	signal(SIGINT, sigint_handler);
	
	// logz
	pthread_t log_thread;
	if (pthread_create(&log_thread, NULL, logz, NULL) != 0) {
		printf("something died: no thread created\n");
		return 1;
	}

	// writing to log?
	int c;
	while (run_flag && (c = getchar() != EOF)) {
		pthread_mutex_lock(&buffer_mutex);
		if (buffer_index < BUFFER_SIZE) {
			buffer[buffer_index++] = (char)c;
		} else {
			printf("buffer full\n");
		}
		pthread_mutex_unlock(&buffer_mutex);
	}

	// mlep
	pthread_join(log_thread, NULL);
	pthread_mutex_destroy(&buffer_mutex);
	return 0;
}
