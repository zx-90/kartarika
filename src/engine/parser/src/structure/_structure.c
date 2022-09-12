/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"

bool kar_parser_extern_brackets(KarToken* token, KarArray* errors);
bool kar_parser_split_by_lines(KarToken* token, KarArray* errors);
bool kar_parser_erase_empty_lines(KarToken* token);
bool kar_parser_split_by_blocks(KarToken* token, KarArray* errors);


bool kar_parser_parse_structure(KarToken* token, KarArray* errors) {
	bool b = true;
	b = b && kar_parser_extern_brackets(token, errors);
	b = b && kar_parser_split_by_lines(token, errors);
	b = b && kar_parser_erase_empty_lines(token);
	b = b && kar_parser_split_by_blocks(token, errors);
	return b;
}
