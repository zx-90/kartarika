/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021,2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdio.h>

#include "core/console_system.h"
#include "model/module.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "generator/generator.h"

int main(int argc, char** argv) {
	kar_console_init();
	if (argc != 2) {
		fprintf(stderr, "Ошибка. Необходимо имя файла для запуска.\n");
		return -1;
	}
	char* filename = argv[1];
	printf("%s\n", filename);
	
	KarStream* file = kar_stream_create(filename);
	KarModule* module = kar_module_create(filename);
	
	if (!kar_lexer_run(file, module)) {
		fprintf(stderr, "Ошибка при парсинге файла в лексере.\n");
		kar_module_print_errors(module);
		kar_module_free(module);
		kar_stream_free(file);
		return 1;
	}
	
	if (!kar_parser_run(module)) {
		fprintf(stderr, "Ошибка при парсинге файла в парсере.\n");
		kar_module_free(module);
		kar_stream_free(file);
		return 1;
	}
	kar_token_print(module->token, stdout);
	
	kar_generator_run(module);
	
	kar_module_free(module);
	kar_stream_free(file);

	return 0;

}
