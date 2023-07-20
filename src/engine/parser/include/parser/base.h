/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PARSER_BASE_H
#define KAR_PARSER_BASE_H

#include "model/project_error_list.h"

typedef enum {
	KAR_PARSER_STATUS_ERROR,
	KAR_PARSER_STATUS_PARSED,
	KAR_PARSER_STATUS_NOT_PARSED
} KarParserStatus;

bool kar_parser_is_expression(KarTokenType type);
bool kar_token_is_name(KarTokenType type);
bool kar_token_is_type(KarTokenType type);
bool kar_parser_check_ifelse(KarToken* token);
bool kar_parser_check_else(KarToken* token);

bool kar_parser_parse_algorithm(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);

#endif // KAR_PARSER_BASE_H
