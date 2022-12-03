#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char** argv) {

	printf("******************\n");
	printf("�������� 1(0) %d\n", getpid());
	printf("������������ %d\n", getppid());
	printf("******************\n");

	pid_t pid_2_1;

	if ((pid_2_1 = fork()) == 0) {

		printf("������� %d ���������, pid=%d, ppid%d \n", getppid(), getpid(), getppid());
		//printf("������� 2(1) %d ��������� Ps\n", getpid());
		system("ps");

		pid_t pid_3_2;

		if ((pid_3_2 = fork()) == 0) {

			printf("������� %d ���������, pid=%d, ppid%d \n", getppid(), getpid(), getppid());
			//*printf("�������� 3(2) %d\n", getpid());
			//*printf("������������ %d\n", getppid());

			pid_t pid_5_3;

			if ((pid_5_3 = fork()) == 0) {

				printf("������� %d ���������, pid=%d, ppid%d \n", getppid(), getpid(), getppid());
				//*printf("�������� 5(3) %d\n", getpid());
				//*printf("������������ %d\n", getppid());
				printf("������� %d ��������� ������\n", getpid());

				exit(0);
			}

			wait(0);

			printf("������� %d ��������� ������\n", getpid());

			exit(0);
		}

		wait(0);

		pid_t pid_4_2;

		if ((pid_4_2 = fork()) == 0) {

			printf("������� %d ���������, pid=%d, ppid%d \n", getppid(), getpid(), getppid());
			//*printf("�������� 4(2) %d\n", getpid());
			//*printf("������������ %d\n", getppid());

			pid_t pid_6_4;

			if ((pid_6_4 = fork()) == 0) {

				printf("������� %d ���������, pid=%d, ppid%d \n", getppid(), getpid(), getppid());
				//*printf("�������� 6(4) %d\n", getpid());
				//*printf("������������ %d\n", getppid());

				pid_t pid_7_6;

				if ((pid_7_6 = fork()) == 0) {

					printf("������� %d ���������, pid=%d, ppid%d \n", getppid(), getpid(), getppid());
					//*printf("�������� 7(6) %d\n", getpid());
					//*printf("������������ %d\n", getppid());
					printf("������� %d ��������� ������\n", getpid());

					exit(0);
				}

				wait(0);

				printf("������� % d ��������� ������\n", getpid());

				exit(0);
			}

			wait(0);

			printf("������� %d ��������� ������\n", getpid());

			exit(0);
		}

		wait(0);

		printf("������� %d ��������� ������\n", getpid());

		exit(0);
	}

	wait(0);

	printf("������� %d ��������� ������\n", getpid());

	return 0;
}