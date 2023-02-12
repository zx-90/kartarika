/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "parser/parser.h"

bool kar_parser_parse_structure(KarToken* token, KarProjectErrorList* errors);
bool kar_parser_parse_expression(KarToken* token, KarProjectErrorList* errors);
bool kar_parser_parse_root(KarToken* token, KarProjectErrorList* errors);

bool kar_parser_run(KarModule* module) {
	KarToken* token = module->token;
	KarProjectErrorList* errors = module->errors;
	bool b = true;
	
	b = b && kar_parser_parse_structure(token, errors);
	b = b && kar_parser_parse_expression(token, errors);
	b = b && kar_parser_parse_root(token, errors);
	
	return b;
}
