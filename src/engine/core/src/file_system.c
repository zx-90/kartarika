/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/file_system.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include "core/alloc.h"
#include "core/string.h"

bool k_file_system_is_file(const char* path) {
	struct stat sts;
	errno = ENOENT;
	return (stat(path, &sts) == -1 && errno == ENOENT);
}

bool k_file_system_is_directory(const char* path) {
	errno = ENOENT;
	DIR* dir = opendir(path);
	bool result = dir && errno == ENOENT;
	closedir(dir);
	return result;
}

char** k_file_create_directory_list(const char* path, size_t* count) {
	DIR *dir = opendir(path);
	if (!dir) {
		return NULL;
	}
	struct dirent* ent;

	*count = 0;
	while ((ent = readdir(dir)) != NULL) {
		(*count)++;
	}
	
	K_CREATES(result, char*, *count);
	if (!result) {
		return NULL;
	}
	
	rewinddir(dir);
	
	size_t i = 0;
	while ((ent = readdir(dir)) != NULL) {
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
			(*count)--;
			continue;
		}
		K_CREATES(element, char, strlen(ent->d_name) + 1);
		if (!element) {
			while (i--) {
				K_FREE(result[i]);
			}
			K_FREE(result);
			return NULL;
		}
		strcpy(element, ent->d_name);
		result[i] = element;
		i++;
	}
	
	closedir (dir);
	return result;
}

char** k_file_create_absolute_directory_list(const char* path, size_t* count) {
	char** result = k_file_create_directory_list(path, count);
	if (!result) {
		return NULL;
	}
	
	// TODO: Здесь надо скачивать это соединение "/" через функции ОС.
	const char* path2 = k_string_concat(path, "/");
	if (!path2) {
		k_string_list_free(result, *count);
		*count = 0;
		return NULL;
	}
	
	size_t i;
	for (i = 0; i < *count; i++) {
		char* file_name = result[i];
		char* absolute_file_name = k_string_concat(path2, file_name);
		K_FREE(result[i]);
		result[i] = absolute_file_name;
	}
	return result;
}

char* k_file_load(const char* path) {
	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		return NULL;
	}
	
	fseek(f, 0, SEEK_END);
	size_t size = (size_t)ftell(f);
	
	fseek(f, 0, SEEK_SET);
	K_CREATES(result, char, size + 1);
	if (size != fread(result, sizeof(char), size, f)) {
		K_FREE(result);
		return NULL;
	}
	
	fclose(f);
	result[size] = 0;
	
	return result;
}

static char* working_dir = NULL;

const char* k_file_get_working_dir() {
	if (working_dir) {
		return working_dir;
	}
	working_dir = getcwd(NULL, 0);
	return working_dir;
}
