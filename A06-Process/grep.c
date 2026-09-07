#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
	// take in command line arguments
	if (argc < 4) {
		printf("usage: %s <N_processes> <keyword> <file1> ...", argv[0]);
		return 1;
	}

	int num_processes = atoi(argv[1]);
	if (num_processes <= 0) {
		printf("Please enter at least 1+ processes.\n");
		return 1;
	}

	int num_files = argc - 3;
	char *keyword = argv[2];
	if (num_processes > num_files) {
		printf("The number of processes (%d) is greater than the number of files (%d). Setting the number of processes to the number of files.\n", num_processes, num_files);
		num_processes = num_files;
	}
	printf("Searching %d files for keyword: %s\n", num_files, keyword);

	// setup
	int files_per_process = num_files / num_processes;
	int remaining_files = num_files % num_processes;
	int start_idx = 0;
	int end_idx = 0;

	// calculate the time
	struct timeval start_time, end_time;
	gettimeofday(&start_time, NULL);

	// distribute files to processes
	for (int i = 0; i < num_processes; i++) {
		int files_for_process = files_per_process;	
		if (i < remaining_files) {
			files_for_process++;
		}

		end_idx = start_idx + files_for_process;
		if (files_for_process == 0) {
            		start_idx = end_idx;
            		continue;
        	}

		pid_t pid = fork();
		if (pid < 0) {
			printf("Fork failed :(\n");
			return 1;
		}
		if (pid == 0) {
			int count;
			char line[1024];
			int files_assigned = end_idx - start_idx;
			int real_start_idx = start_idx + 3;
			int real_end_idx = end_idx + 3;
			pid = getpid();
			printf("Process [%d] searching %d files (%d to %d)\n",
                   		pid, files_assigned, real_start_idx, real_end_idx);
		
			for (int j = start_idx; j < end_idx; j++) {
				char *filename = argv[j+3];
				FILE *fptr = fopen(filename, "r");
				if (!fptr) {
					printf("Could not open file %s for process [%d], skipping.\n", filename, pid);
					continue;
				}

				while (fgets(line, 1024, fptr) != NULL) {
                    			if (strstr(line, keyword) != NULL) {
                        			line[strcspn(line, "\n")] = 0;
                        			printf("%d) %s:%s\n", pid, filename, line);
                        			count++;
                    			}
                		}
                		fclose(fptr);
			}
			printf("Process [%d] found %d lines containing keyword: %s\n", pid, count, keyword);
			
			if (count > 255) {
				exit(255);
			} else {
				exit(count);
			}
		}
		start_idx = end_idx;
	}

	// wait for child to finish
	int total_count = 0;
	int status;
	int complete = 0;
	pid_t pid_wait;

	while (complete < num_processes) {
		pid_wait = waitpid(-1, &status, 0);
		if (pid_wait > 0) {
			complete++;
			if (WIFEXITED(status)) {
				int child = WEXITSTATUS(status);
				total_count++;
				printf("Process [%d] found %d lines containing keyword: %s\n", pid_wait, child, keyword);
			} else {
				printf("something probably happened.\n");
			}
		} else if (pid_wait < 0) {
			printf("waitpid failed\n");
			break;
		}
	}
	printf("Total occurances: %d\n", total_count);

	// calculate time
	gettimeofday(&end_time, NULL);
	long elapsed_sec = end_time.tv_sec - start_time.tv_sec;
    	long elapsed_usec = end_time.tv_usec - start_time.tv_usec;
    	double elapsed_time = elapsed_sec + (double)elapsed_usec / 1000000.0;
    	printf("Elapsed time is %.6f\n", elapsed_time);
	return 0;
}
