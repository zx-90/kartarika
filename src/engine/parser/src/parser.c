/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "parser/parser.h"

#include "parser/elements/_elements.h"

bool kar_parser_run(KarModule* module) {
	KarToken* token = module->token;
	bool b = true;
	b = b && kar_parser_split_by_lines(token);
	b = b && kar_parser_split_by_blocks(token);
	b = b && kar_parser_extern_brackets(token);
	b = b && kar_parser_make_path(token);
	
	b = b && kar_parser_remove_spaces(token);
	
	b = b && kar_parser_make_return(token);
	b = b && kar_parser_make_function(token);
	return b;
}
