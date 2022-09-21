/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"

bool kar_token_is_name(KarTokenType type) {
    return
        kar_token_type_is_identifier(type);
}

bool kar_token_is_type(KarTokenType type) {
    return
        kar_token_type_is_identifier(type) ||
        kar_token_type_is_variable(type) ||
        type == KAR_TOKEN_SIGN_GET_FIELD;
}

bool kar_parser_is_expression(KarTokenType type)
{
	return
		kar_token_type_is_identifier(type) ||
		kar_token_type_is_value(type) ||
		kar_token_type_is_variable(type) ||
		kar_token_type_is_sign(type);
}