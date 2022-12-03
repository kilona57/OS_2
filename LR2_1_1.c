#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


int GetFileSize(const struct dirent* element, const char* current_path) 
{
	struct stat stat_buffer;
	char* path;

	asprintf(&path, "%s/%s", current_path, element->d_name);
	stat(path, &stat_buffer);
	free(path);
	return stat_buffer.st_size;
}

static int IsDir(const struct dirent* element) 
{
	return (element->d_type == DT_DIR);
}

static int IsFile(const struct dirent* element) 
{
	return (element->d_type == DT_REG);
}

size_t ReturnFileSize(FILE* f) {

	size_t pos = ftell(f);
	size_t size;

	fseek(f, 0L, SEEK_END);
	size = ftell(f);
	fseek(f, pos, SEEK_SET);

	return size;
}

char* FullPath(const char* string_1, const char* string_2) {

	char* output;

	asprintf(&output, "%s/%s", string_1, string_2);

	return output;
}

int FileExists(const char* path, const char* filename) 
{
	DIR* dir_stream;
	struct dirent* entry;

	if ((dir_stream = opendir(path)) == NULL) 
	{
		printf("Error opening directory stream on %s\n", path);
		return -1;
	}

	while (entry = readdir(dir_stream))
	{
		if (strcmp(entry->d_name, filename) == 0) 
		{
			closedir(dir_stream);
			return 1;
		}
	}

	closedir(dir_stream);
	return 0;
}

int CopyFile(const char* destination_path, const char* source_path, const char* filename) {

	char* full_source_path, * full_destination_path;
	struct stat statbuf;

	asprintf(&full_source_path, "%s/%s", source_path, filename);
	asprintf(&full_destination_path, "%s/%s", destination_path, filename);

	stat(full_source_path, &statbuf);

	if (S_ISDIR(statbuf.st_mode))
		mkdir(full_destination_path, statbuf.st_mode);
	else if (S_ISREG(statbuf.st_mode)) {

		FILE* source_object;
		FILE* destination_object;
		int temp_char;
		char* buf;

		if ((source_object = fopen(full_source_path, "r")) == NULL) {

			printf("Error opening %s\n", full_source_path);
			return -1;
		}
		if ((destination_object = fopen(full_destination_path, "w")) == NULL) {

			printf("Error opening %s\n", full_destination_path);
			return -1;
		}
		buf = malloc(ReturnFileSize(source_object) + 1);
		temp_char = fread(buf, 1, ReturnFileSize(source_object) + 1, source_object);
		fwrite(buf, 1, temp_char, destination_object);
		free(buf);

		chmod(full_destination_path, statbuf.st_mode);

		fclose(source_object);
		fclose(destination_object);

		return 1;

	}

}

void CopyDirContents(const char* destination_path, const char* source_path, int max_thread_count) 
{
	int available_thread_count = max_thread_count;

	CopyDirContents(destination_path, source_path, available_thread_count);

	struct dirent** file_buffer;
	struct dirent** dir_buffer;
	int fcount, dcount;

	printf("In directory %s\n", source_path);

	fcount = scandir(source_path, &file_buffer, IsFile, alphasort);
	dcount = scandir(source_path, &dir_buffer, IsDir, alphasort);


	if (fcount > 0)
		for (int i = 0; i < fcount; i++)
		{
			if (FileExists(destination_path, file_buffer[i]->d_name) == 0) {

				if (available_thread_count == 0)
				{
					wait(0);
					available_thread_count++;
				}

				available_thread_count--;

				pid_t pid;

				if ((pid = fork()) < 0) {

					printf("Error creating new process\n");
					return;
				}
				else if (pid == 0)
				{
					printf("Copying file: PID %d, Process available %d], Size %d Kb, Name [%s] \n", getpid(), available_thread_count, GetFileSize(file_buffer[i], source_path) / 1024, file_buffer[i]->d_name);

					CopyFile(destination_path, source_path, file_buffer[i]->d_name);

					exit(0);
				}
			}
		}


	if (dcount > 0)
		for (int i = 0; i < dcount; i++)
		{
			if (strcmp(dir_buffer[i]->d_name, ".") == 0 || strcmp(dir_buffer[i]->d_name, "..") == 0)
				continue;
			else
			{
				if (FileExists(destination_path, dir_buffer[i]->d_name) == 0)
				{
					CopyFile(destination_path, source_path, dir_buffer[i]->d_name);
				}
				CopyDirContents(FullPath(destination_path, dir_buffer[i]->d_name), FullPath(source_path, dir_buffer[i]->d_name), available_thread_count);
			}
		}

	free(file_buffer);
	free(dir_buffer);
}

int main(int argc, char* argv[]) 
{
	if (argc < 4) 
	{
		printf("*****ERROR***** \nIncorrect number of parameters in function main(): 3 expected but only %d given", argc - 1);
		return -1;
	}

	CopyDirContents(argv[1], argv[2], atoi(argv[3]));

	return 0;
}