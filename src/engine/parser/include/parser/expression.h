/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PARSER_EXPRESSION_H
#define KAR_PARSER_EXPRESSION_H

bool kar_parser_make_path(KarToken* token, KarArray* errors);
bool kar_parser_make_operands(KarToken* token, KarArray* errors);

bool kar_parser_is_expression(KarToken* token);

#endif // KAR_PARSER_EXPRESSION_H
