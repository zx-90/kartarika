/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "tester/test_suite.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "core/string.h"
#include "core/file_system.h"

static KarError* check_for_subdirs(char** files, size_t count) {
	size_t i;
	for (i = 0; i < count; i++) {
		if (kar_file_system_is_directory(files[i])) {
			continue;
		} else {
			if (kar_file_system_is_file(files[i]) && !strcmp(kar_file_system_get_basename(files[i]), KAR_COMMENT_FILENAME)) {
				continue;
			} else {
				return kar_error_register(1, "Объект %s не является файлом.", files[i]);
			}
		}
	}
	return NULL;
}

static KarError* run_dir(const char* path) {
	size_t count;
	char** files = kar_file_create_absolute_directory_list(path, &count);
	if (!files) {
		return kar_error_get_last();
	}
	if (count == 0) {
		printf("Предупреждение. Каталог %s пуст.\n", path);
		kar_string_list_free(files, count);
		return NULL;
	}
	
	KarError* error = check_for_subdirs(files, count);
	if (!error) {
		//printf("КАТАЛОГ %s\n", path);
		size_t i;
		for (i = 0; i < count; i++) {
			if (kar_file_system_is_directory(files[i])) {
				error = run_dir(files[i]);
				if (error) {
					kar_string_list_free(files, count);
					return error;
				}
			}
		}
	} else {
		printf("ТЕСТ %s\n", path);
		KarTest* tf = kar_test_create();
		KarError* result = kar_test_run(tf, path);
		kar_test_free(tf);
		kar_string_list_free(files, count);
		return result;
	}
	
	kar_string_list_free(files, count);
	return NULL;
}

KarError* kar_test_suite_run(const char* path) {
	if (!kar_file_system_is_directory(path)) {
		return kar_error_register(1, "Ошибка при поиске тестов. Объект \"%s\" не является каталогом.", path);
	}
	
	return run_dir(path);
}
