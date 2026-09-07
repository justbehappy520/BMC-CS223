#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_RESET   "\x1b[0m"

#define ANSI_BACKGROUND_BLACK   "\x1b[40m"
#define ANSI_BACKGROUND_RED     "\x1b[41m"
#define ANSI_BACKGROUND_GREEN   "\x1b[42m"
#define ANSI_BACKGROUND_YELLOW  "\x1b[43m"
#define ANSI_BACKGROUND_BLUE    "\x1b[44m"
#define ANSI_BACKGROUND_MAGENTA "\x1b[45m"
#define ANSI_BACKGROUND_CYAN    "\x1b[46m"
#define ANSI_BACKGROUND_WHITE   "\x1b[47m"

#define ANSI_STYLE_BOLD         "\x1b[1m"
#define ANSI_STYLE_ITALIC       "\x1b[3m"
#define ANSI_STYLE_UNDERLINE    "\x1b[4m"

#define MAX_VAL 64

int main()
{
	// print welcome message
	printf(ANSI_COLOR_GREEN "****************************************\n");
        printf(
    		ANSI_COLOR_GREEN "** "
    		ANSI_STYLE_BOLD ANSI_COLOR_WHITE ANSI_BACKGROUND_MAGENTA
    		" Welcome to my shell!! >:3 (v1.0) " 
    		ANSI_RESET
    		ANSI_COLOR_GREEN " **\n"
	);
	printf(ANSI_COLOR_GREEN "****************************************\n" ANSI_RESET);
	
	// input line
	char *input_line;

	// basic idea: infinite looping between read, parse, and execute
	while (1) {
		// ****************
		// print a prompt!!
		// ****************
		// helpful functions: gethostname(), getcwd(), getpwuid(geteuid())
		
		// prompt
		char prompt_buffer[MAX_VAL];
		
		// print prompt
		snprintf(prompt_buffer, sizeof(prompt_buffer), ANSI_STYLE_ITALIC ANSI_COLOR_MAGENTA "[myshell] glumball's shell > " ANSI_RESET);
        	
		// ****************************
		// read in command line input!!
		// ****************************
                // helpful functions: readline()

		// read input
		input_line = readline(prompt_buffer);		
		
		// check for end of file
		if (input_line == NULL) {
			printf("\nExiting loop!! Bai~\n");
			break;
		}
		
		// check if the input is empty
		if (strlen(input_line) == 0) {
            		free(input_line);
            		continue;
        	}

		// save to history
		add_history(input_line);

		// check for exit
		if (strcmp(input_line, "exit") == 0) {
			free(input_line);
			break;
		}
		
		// **************************
		// parsing and tokenization!!
		// **************************
		// helpful functions: strtok() or something similar
		
		// store tokens into a null-terminated argument array
		char* tokens[MAX_VAL];

		// split command line into tokens
		int idx = 0;
		char* token = strtok(input_line, " ");
		while (token != NULL) {
			if (idx >= MAX_VAL-1) {
				break;
			}
			tokens[idx] = token;
			token = strtok(NULL, " ");
			idx++;
		}
		tokens[idx] = NULL;

		// *****************************
		// execution!! dun dun DUNNNNNNN
		// *****************************
		// helpful functions: fork(), execvp()

		// fork a new process!! yayyyyyy :(
		pid_t pid = fork();

		// make sure the fork is fine!!
		if (pid < 0) {
			printf("Oh dear, fork failed :(\n");
		}

		// exec
		else if (pid == 0) {
			if (execvp(tokens[0], tokens) < 0) {
				printf("%s: command not found\n", tokens[0]);
				exit(1);
			}
		}

		// parent
		else {
			int status; // this is for the child
			if (waitpid(pid, &status, 0) < 0) {
				printf("Oh dear, waitpid failed :(\n");
			}
			if (WIFSIGNALED(status)) {
				int signal = WTERMSIG(status);
				printf("BOO!! %s\n", strsignal(signal));
				if (WCOREDUMP(status))
				{
					printf("Oh dear, core dumped\n");
				}
			}
		}

		// *****************************
	        // tie things up nice and neat!!
		// *****************************	
		free(input_line);
	}
	return 0;
}

