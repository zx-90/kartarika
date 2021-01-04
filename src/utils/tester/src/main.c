/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "tester/test_suite.h"

#include <stdio.h>

#include "core/alloc.h"
#include "core/string.h"
#include "core/file_system.h"

int main() {
	// TODO: Здесь объединение подкаталогов надо вынести в отдельный модуль работы с файловой системой.
	char* test_path = kar_string_create_concat(kar_file_get_working_dir(), "/тесты");
	printf("Запуск каталога с тестами: %s\n", test_path);
	
	KarError* error = kar_test_suite_run(test_path);
	if (error) {
		KAR_FREE(test_path);
		printf("Ошибка %ld: %s\n", error->number, error->description);
		return 1;
	}
	
	KAR_FREE(test_path);
	return 0;
}
