/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PARSER_BASE_H
#define KAR_PARSER_BASE_H

typedef enum {
	KAR_PARSER_STATUS_ERROR,
	KAR_PARSER_STATUS_PARSED,
	KAR_PARSER_STATUS_NOT_PARSED
} KarParserStatus;

bool kar_parser_is_expression(KarToken* token);
bool kar_token_is_name(KarToken* token);
bool kar_token_is_type(KarToken* token);

#endif // KAR_PARSER_BASE_H
