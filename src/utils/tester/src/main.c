/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021,2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "tester/test_suite.h"

#include <stdio.h>

#include "core/alloc.h"
#include "core/string.h"
#include "core/file_system.h"
#include "core/console_system.h"

int main(int argc, char **argv) {
	// TODO: Добавить параметр для просмотра всех сообщений о ошибках.
	// TODO: Добавить параметр для вывода структуры токенов.
	kar_console_init();
	char* test_path;
	if (argc < 2) {
		test_path = "тесты";
	} else if (argc == 2) {
		test_path = argv[1];
	} else {
		printf("Ошибка. Количество аргументов запуска должно быть 0 или 1.");
		return 1;
	}
	
	printf("Запуск каталога с тестами: %s\n", test_path);
	KarError* error = kar_test_suite_run(test_path);
	if (error) {
		printf("Ошибка %ld: %s\n", error->number, error->description);
		return 1;
	}
	
	return 0;
}
