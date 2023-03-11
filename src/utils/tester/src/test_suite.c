/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
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

static KarError* check_for_subdirs(KarStringList* files) {
	size_t i;
	for (i = 0; i < kar_string_list_count(files); i++) {
		KarString* filename = kar_string_list_get(files, i);
		if (kar_file_system_is_directory(filename)) {
			continue;
		} else {
			if (kar_file_system_is_file(filename) && kar_string_equal(kar_file_system_get_basename(filename), KAR_COMMENT_FILENAME)) {
				continue;
			} else {
				return kar_error_register(1, "Объект %s не является файлом.", filename);
			}
		}
	}
	return NULL;
}

static KarError* run_dir(const KarString* path) {
	KarStringList* files = kar_file_create_absolute_directory_list(path);
	if (!files) {
		return kar_error_get_last();
	}
	if (kar_string_list_count(files) == 0) {
		printf("Предупреждение. Каталог %s пуст.\n", path);
		kar_string_list_free(files);
		return NULL;
	}
	
	KarError* error = check_for_subdirs(files);
	if (!error) {
		size_t i;
		for (i = 0; i < kar_string_list_count(files); i++) {
			KarString* filename = kar_string_list_get(files, i);
			if (kar_file_system_is_directory(filename)) {
				error = run_dir(filename);
				if (error) {
					kar_string_list_free(files);
					return error;
				}
			}
		}
	} else {
		printf("ТЕСТ %s\n", path);
		KarTest* tf = kar_test_create();
		KarError* result = kar_test_run(tf, path);
		kar_test_free(tf);
		kar_string_list_free(files);
		return result;
	}
	
	kar_string_list_free(files);
	return NULL;
}

KarError* kar_test_suite_run(const KarString* path) {
	if (!kar_file_system_is_directory(path)) {
		return kar_error_register(1, "Ошибка при поиске тестов. Объект \"%s\" не является каталогом.", path);
	}
	
	return run_dir(path);
}
