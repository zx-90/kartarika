/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "parser/parser.h"

#include "parser/elements.h"

bool kar_parser_run(KarModule* module) {
	KarToken* token = module->token;
	KarArray* errors = &module->errors;
	bool b = true;
	
	// Парсинг структуры.
	b = b && kar_parser_extern_brackets(token, errors);
	b = b && kar_parser_split_by_lines(token, errors);
	b = b && kar_parser_split_by_blocks(token, errors);
	
	// Парсинг до удаления пробелов.
	b = b && kar_parser_make_path(token, errors);
	
	// Удаление пробелов.
	b = b && kar_parser_remove_spaces(token);
	
	// Парсинг после удаления пробелов.
	b = b && kar_parser_make_operands(token, errors);
	b = b && kar_parser_make_variable(token, errors);
	b = b && kar_parser_make_return(token);
	b = b && kar_parser_make_method(token, errors);
	
	return b;
}
