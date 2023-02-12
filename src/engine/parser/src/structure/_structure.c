/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"

bool kar_parser_extern_brackets(KarToken* token, KarProjectErrorList* errors);
bool kar_parser_split_by_lines(KarToken* token, KarProjectErrorList* errors);
bool kar_parser_erase_empty_lines(KarToken* token);
bool kar_parser_split_by_blocks(KarToken* token, KarProjectErrorList* errors);


bool kar_parser_parse_structure(KarToken* token, KarProjectErrorList* errors) {
	bool b = true;
	b = b && kar_parser_extern_brackets(token, errors);
	b = b && kar_parser_split_by_lines(token, errors);
	b = b && kar_parser_erase_empty_lines(token);
	b = b && kar_parser_split_by_blocks(token, errors);
	return b;
}
