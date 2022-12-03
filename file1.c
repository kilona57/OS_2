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

struct stat get_stat_buffer(const struct dirent* element, const char* current_path) {

	struct stat stat_buffer;
	char* path;

	asprintf(&path, "%s/%s", current_path, element->d_name);

	stat(path, &stat_buffer);

	free(path);

	return stat_buffer;

}

int get_file_size(const struct dirent* element, const char* current_path) {

	return get_stat_buffer(element, current_path).st_size;

}

static int isdir(const struct dirent* element) {

	return (element->d_type == DT_DIR);

}

static int isfile(const struct dirent* element) {

	return (element->d_type == DT_REG);

}

size_t return_file_size(FILE* f) {

	size_t pos = ftell(f);
	size_t size;

	fseek(f, 0L, SEEK_END);
	size = ftell(f);
	fseek(f, pos, SEEK_SET);

	return size;
}

char* path_concat(const char* string_1, const char* string_2) {

	char* output;

	asprintf(&output, "%s/%s", string_1, string_2);

	return output;
}

int check_filename_presence(const char* path, const char* filename) {

	DIR* dir_stream;
	struct dirent* entry;

	if ((dir_stream = opendir(path)) == NULL) {
		printf("Error opening directory stream on %s\n", path);
		return -1;
	}

	while (entry = readdir(dir_stream))
		if (strcmp(entry->d_name, filename) == 0) {

			closedir(dir_stream);
			return 1;
		}

	closedir(dir_stream);
	return 0;

}

int copy_object(const char* destination_path, const char* source_path, const char* filename) {

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
		char *buf;

		if ((source_object = fopen(full_source_path, "r")) == NULL) {

			printf("Error opening %s\n", full_source_path);
			return -1;
		}
		if ((destination_object = fopen(full_destination_path, "w")) == NULL) {

			printf("Error opening %s\n", full_destination_path);
			return -1;
		}

		//while ((temp_char = getc(source_object)) != EOF)
		//	fputc(temp_char, destination_object);
		//while ((temp_char = fread(buf, 1, 1024, source_object)) == 1024)
		//	fwrite(buf, 1, 1024, destination_object);
		//fwrite(buf, 1, temp_char, destination_object);

		buf = malloc(return_file_size(source_object) + 1);
		temp_char = fread(buf, 1, return_file_size(source_object) + 1, source_object);
		fwrite(buf, 1, temp_char, destination_object);
		free(buf);

		chmod(full_destination_path, statbuf.st_mode);

		fclose(source_object);
		fclose(destination_object);

		return 1;

	}

}

void copy_dir_contents_recursive(const char* destination_path, const char* source_path, int* available_thread_count) {

	struct dirent** file_buffer;
	struct dirent** dir_buffer;
	int file_count, dir_count;

	
	printf("In directory %s\n", source_path);

	file_count = scandir(source_path, &file_buffer, isfile, alphasort);
	dir_count = scandir(source_path, &dir_buffer, isdir, alphasort);


	if (file_count > 0)
		for (int i = 0; i < file_count; i++)
			if (check_filename_presence(destination_path, file_buffer[i]->d_name) == 0) {

				if ((*available_thread_count) == 0) {

					wait(0);
					(*available_thread_count)++;
				}

				(*available_thread_count)--;

				pid_t pid;

				if ((pid = fork()) < 0) {

					printf("Error creating new process\n");
					return;
				}
				else if (pid == 0) {

				printf("Copying file: PID %d, Process available %d], Size %d Kb, Name [%s] \n", getpid(), (*available_thread_count), get_file_size(file_buffer[i], source_path) / 1024, file_buffer[i]->d_name);

				copy_object(destination_path, source_path, file_buffer[i]->d_name);


				exit(0);

				}


			}


	if (dir_count > 0)
		for (int i = 0; i < dir_count; i++)
			if (strcmp(dir_buffer[i]->d_name, ".") == 0 || strcmp(dir_buffer[i]->d_name, "..") == 0)
				continue;
			else {

				if (check_filename_presence(destination_path, dir_buffer[i]->d_name) == 0) {


					copy_object(destination_path, source_path, dir_buffer[i]->d_name);
				}

				copy_dir_contents_recursive(path_concat(destination_path, dir_buffer[i]->d_name), path_concat(source_path, dir_buffer[i]->d_name), available_thread_count);
			}

	free(file_buffer);
	free(dir_buffer);

}

void copy_dir_contents(const char* destination_path, const char* source_path, int max_thread_count) {

	int* available_thread_count = malloc(sizeof(int));
	*available_thread_count = max_thread_count;

	copy_dir_contents_recursive(destination_path, source_path, available_thread_count);

}

int main(int argc, char* argv[]) {

	if (argc < 4) {

		printf("*****ERROR***** \nIncorrect number of parameters in function main(): 3 expected but only %d given", argc - 1);
		return -1;
	}

	copy_dir_contents(argv[1], argv[2], atoi(argv[3]));

	return 0;
}
