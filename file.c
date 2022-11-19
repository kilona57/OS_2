#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
using namespace std;
int main()
{
    system("date +%T.%3N");

	pid_t child_a,child_b;

	child_a = fork();
	if (child_a == 0) {
		wait(nullptr);
		printf("Child A pid =%d\nppid (A) =%d\n\n", getpid(), getppid());
		system("date +%T.%3N");
	}
	else {
		system("date +%T.%3N");
		child_b = fork();
		if (child_b == 0) {
			
			printf("Child B pid =%d\nppid (B) =%d\n\n", getpid(), getppid());
		}
		else {
			printf("Parent pid =%d \nppid =%d \n\n", getpid(), getppid());
			system("ps -x");
			return EXIT_SUCCESS;
		}
	}
}