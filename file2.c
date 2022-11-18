#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{

	printf("******************\n");
	printf("Дочерний 1(0) %d\n", getpid());
	printf("Родительский %d\n", getpid());
	printf("******************\n");


	pid_t pid_2_1;

	if ((pid_2_1 = fork()) == 0)
	{
		printf("Процесс % d породил процесс %d\n", getpid(), getpid());
		printf("Дочерний 2(1) %d\n", getpid());
		printf("Родительский %d\n", getpid());
		printf("Процесс 2(1) %d выполняет Ps\n", getpid());
		system("ps");

		pid_t pid_3_2;

		if ((pid_3_2 = fork()) == 0)
		{
			printf("Процесс % d породил процесс %d\n", getpid(), getpid());
			printf("Дочерний 3(2) %d\n", getpid());
			printf("Родительский %d\n", getpid());
			
			pid_t pid_5_3;

			if ((pid_5_3 = fork()) == 0)
			{
				printf("Процесс % d породил процесс %d\n", getpid(), getpid());
				printf("Дочерний 5(3) %d\n", getpid());
				printf("Родительский %d\n", getpid());
				printf("Дочерний 5(3) %d завершает работу\n", getpid());

				exit(0);
			}

			wait(0);

			printf("Дочерний 3(2) %d завершает работу\n", getpid());
			
			exit(0);
		}

		wait(0);		

		pid_t pid_4_2;

		if ((pid_4_2 = fork()) == 0)
		{
			printf("Процесс % d породил процесс %d\n", getpid(), getpid());
			printf("Дочерний 4(2) %d\n", getpid());
			printf("Родительский %d\n", getpid());

			pid_t pid_6_4;

			if ((pid_6_4 = fork()) == 0)
			{
				printf("Процесс % d породил процесс %d\n", getpid(), getpid());
				printf("Дочерний 6(4) %d\n", getpid());
				printf("Родительский %d\n", getpid());
				
				pid_t pid_7_6;

				if ((pid_7_6 = fork()) == 0)
				{
					printf("Процесс % d породил процесс %d\n", getpid(), getpid());
					printf("Дочерний 7(6) %d\n", getpid());
					printf("Родительский %d\n", getpid());
					printf("Дочерний 7(6) %d завершает работу\n", getpid());

					exit(0);
				}

				wait(0);

				printf("Дочерний 6(4) %d завершает работу\n", getpid());

				exit(0);
			}
			
			wait(0);

			printf("Дочерний 4(2) %d завершает работу\n", getpid());

			exit(0);
		}

		wait(0);

		printf("дочерний 2(1) %d завершает работу\n", getpid());		
		
		exit(0);
	}

	wait(0);

	printf("дочерний 1(0) %d завершает работу\n", getpid());

	return 0;
}
