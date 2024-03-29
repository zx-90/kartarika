/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"

bool kar_parser_make_path_call(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
bool kar_parser_make_method_arguments(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
bool kar_parser_make_operands(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
bool kar_parser_remove_spaces(KarToken* token);
bool kar_parser_concat_single_minus_number(KarToken* token);

bool kar_parser_parse_expression(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	bool b = true;
	b = b && kar_parser_make_path_call(token, moduleName, errors);
	b = b && kar_parser_remove_spaces(token);
	b = b && kar_parser_make_method_arguments(token, moduleName, errors);
	b = b && kar_parser_make_operands(token, moduleName, errors);
	b = b && kar_parser_concat_single_minus_number(token);
	return b;
}
