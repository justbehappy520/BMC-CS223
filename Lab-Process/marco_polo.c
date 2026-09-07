#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

const char* MyName = NULL;

int main(int argc, char* argv[])
{
	MyName = argv[0];
	pid_t pid;
	pid = getpid();
	printf("Hello from %s, (%d)\n", MyName, pid);
	printf("Enter a pid: ");
	char recipient[16];
	scanf("%s", recipient);
	int reci = atoi(recipient);
	char ch = getchar();

	return 0;
}
