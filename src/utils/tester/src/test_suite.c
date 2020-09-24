/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "tester/test_suite.h"

#include <string.h>
#include <stdio.h>

#include "core/file_system.h"

static KError* check_for_subdirs(char** files, size_t count) {
	size_t i;
	for (i = 0; i < count; i++) {
		if (k_file_system_is_directory(files[i])) {
			continue;
		} else {
			if (k_file_system_is_file(files[i]) && !strcmp(files[i], COMMENT_FILENAME)) {
				continue;
			} else {
				return k_error_register(1, "Объект %s не является файлом.", files[i]);
			}
		}
	}
	return NULL;
}

static KError* run_dir(const char* path) {
	printf("Тестирование каталога %s\n", path);
	size_t count;
	char** files = k_file_create_absolute_directory_list(path, &count);
	if (!files) {
		return k_error_get_last();
	}
	if (count == 0) {
		printf("Предупреждение. Каталог пуст.\n");
		return NULL;
	}
	
	KError* error = check_for_subdirs(files, count);
	if (!error) {
		size_t i;
		for (i = 0; i < count; i++) {
			if (k_file_system_is_directory(files[i])) {
				error = run_dir(files[i]);
				if (error) {
					return error;
				}
			}
		}
	} else {
		KTest* tf = k_test_create();
		KError* result = k_test_run(tf, path);
		k_test_free(tf);
		return result;
	}
	
	
	return NULL;
}

KError* k_test_suite_run(const char* path) {

	if (!k_file_system_is_directory(path)) {
		return k_error_register(1, "Ошибка при поиске тестов. Объект \"%s\" не является каталогом.", path);
	}
	
	return run_dir(path);
}
