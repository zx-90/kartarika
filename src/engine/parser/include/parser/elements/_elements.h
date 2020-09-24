/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef PARSER_ELEMENTS_H
#define PARSER_ELEMENTS_H

bool k_parser_split_by_lines(KToken* token);
bool k_parser_split_by_blocks(KToken* token);
bool k_parser_extern_brackets(KToken* token);
bool k_parser_make_path(KToken* token);

bool k_parser_remove_spaces(KToken* token);

bool k_parser_make_return(KToken* token);
bool k_parser_make_function(KToken* token);

#endif // PARSER_ELEMENTS_H
