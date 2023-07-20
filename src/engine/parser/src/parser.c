/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "parser/parser.h"

bool kar_parser_parse_structure(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
bool kar_parser_parse_expression(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
bool kar_parser_parse_root(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);

bool kar_parser_run(KarModule* module, KarProjectErrorList* errors) {
	KarToken* token = module->token;

	bool b = true;
	
    b = b && kar_parser_parse_structure(token, kar_module_get_full_name(module),errors);
    b = b && kar_parser_parse_expression(token, kar_module_get_full_name(module), errors);
    b = b && kar_parser_parse_root(token, kar_module_get_full_name(module), errors);
	
	return b;
}
