/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdio.h>

#include "core/module.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "generator/generator.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Ошибка. Необходимо имя файла для запуска.\n");
		return -1;
	}
	printf("%s\n", argv[1]);
	
	KStream* file = k_stream_create(argv[1]);
	KModule* module = k_module_create(argv[1]);
	
	if (!k_lexer_run(file, module)) {
		fprintf(stderr, "Ошибка при парсинге файла в лексере.\n");
		return 1;
	}
	
	if (!k_parser_run(module)) {
		fprintf(stderr, "Ошибка при парсинге файла в парсере.\n");
		return 1;
	}
	k_token_print(module->token, stdout);
	
	k_generator_run(module);
	
	k_module_free(module);
	k_stream_free(file);

	return 0;

}
