/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PARSER_ELEMENTS_H
#define KAR_PARSER_ELEMENTS_H

bool kar_parser_split_by_lines(KarToken* token);
bool kar_parser_split_by_blocks(KarToken* token);
bool kar_parser_extern_brackets(KarToken* token);
bool kar_parser_make_path(KarToken* token);

bool kar_parser_remove_spaces(KarToken* token);

bool kar_parser_make_return(KarToken* token);
bool kar_parser_make_function(KarToken* token);

#endif // KAR_PARSER_ELEMENTS_H
