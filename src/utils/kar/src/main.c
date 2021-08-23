/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdio.h>

#include "core/module.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "generator/generator.h"
#include "core/console_system.h"

int main(int argc, char** argv) {
	kar_console_init();
	if (argc != 2) {
		fprintf(stderr, "Ошибка. Необходимо имя файла для запуска.\n");
		return -1;
	}
	printf("%s\n", argv[1]);
	
	KarStream* file = kar_stream_create(argv[1]);
	KarModule* module = kar_module_create(argv[1]);
	
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
