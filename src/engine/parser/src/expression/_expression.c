/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"

bool kar_parser_make_call_method(KarToken* token);
bool kar_parser_make_arguments(KarToken* token, KarArray* errors);
bool kar_parser_make_path(KarToken* token, KarArray* errors);
bool kar_parser_make_operands(KarToken* token, KarArray* errors);
bool kar_parser_remove_spaces(KarToken* token);

bool kar_parser_parse_expression(KarToken* token, KarArray* errors) {
	bool b = true;
	b = b && kar_parser_make_call_method(token);
	b = b && kar_parser_make_path(token, errors);
	b = b && kar_parser_remove_spaces(token);
	b = b && kar_parser_make_arguments(token, errors);
	b = b && kar_parser_make_operands(token, errors);
	return b;
}