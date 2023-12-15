/* Copyright © 2020,2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/file_system.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

#include "core/alloc.h"
#include "core/string.h"

const KarString* KAR_FILE_SYSTEM_DELIMETER = "/";

bool kar_file_system_is_file(const KarString* path) {
	struct stat sts;
	return (stat(path, &sts) == 0);
}

bool kar_file_system_is_directory(const KarString* path) {

	DIR* dir = opendir(path);
	bool result = (dir != NULL);
	closedir(dir);
	return result;
}

KarString* kar_file_system_get_basename(KarString* path) {
	return basename(path);
}

KarStringList* kar_file_create_absolute_directory_list(const KarString* path) {
	DIR* dir = opendir(path);
	if (!dir) {
		return NULL;
	}

	KarString* path2 = kar_string_create_concat(path, "/");
	if (!path2) {
		return NULL;
	}
	KarStringList* result = kar_string_list_create();

	struct dirent* ent;
	while ((ent = readdir(dir)) != NULL) {
		if (kar_string_equal(ent->d_name, ".") || kar_string_equal(ent->d_name, "..")) {
			continue;
		}
		KarString* name = kar_string_create(ent->d_name);
		KarString* fullName = kar_string_create_concat(path2, name);
		kar_string_free(name);
		kar_string_list_add(result, fullName);
	}
	closedir(dir);
	kar_string_free(path2);
	
	kar_string_list_sort(result, kar_string_less);
	return result;
}

FILE* kar_file_system_create_handle(const KarString* path) {
	return fopen(path, "r");
}

KarString* kar_file_load(const KarString* path) {
	FILE* f = fopen(path, "rb");
	if (f == NULL) {
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size_t size = (size_t)ftell(f);

	fseek(f, 0, SEEK_SET);
	KAR_CREATES(result, KarString, size + 1);
	if (size != fread(result, sizeof(KarString), size, f)) {
		KAR_FREE(result);
		fclose(f);
		return NULL;
	}

	fclose(f);
	result[size] = 0;

	return result;
}

static KarString* working_dir = NULL;

const KarString* kar_file_get_working_dir() {
	if (working_dir) {
		return working_dir;
	}
	working_dir = getcwd(NULL, 0);
	return working_dir;
}
