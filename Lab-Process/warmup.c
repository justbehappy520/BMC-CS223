#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t ret;
	int status;
	ret = getpid();
	printf("%d] A0\n", ret);
	ret = fork();
	if (ret == 0) {
		ret = getpid();
		printf("%d] B0\n", ret);
		ret = fork();
		if (ret == 0) {
			wait(&status);
			ret = getpid();
			printf("%d] B1\n", ret);
		} else {
			wait(&status);
			ret = getpid();
			printf("%d] C0\n", ret);
		}
	} else {
		wait(&status);
		ret = getpid();
		printf("%d] C1\n", ret);
	}
	ret = getpid();
	printf("%d] Bye\n", ret);
	return 0;
}
