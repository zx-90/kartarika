/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"

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

bool kar_parser_check_ifelse(KarToken* token) {
	if (token->children.count != 5) {
		return false;
	}
	if (kar_token_child(token, 0)->type != KAR_TOKEN_COMMAND_ELSE) {
		return false;
	}
	if (kar_token_child(token, 1)->type != KAR_TOKEN_COMMAND_IF) {
		return false;
	}
	if (!kar_parser_is_expression(kar_token_child(token, 2)->type)) {
		return false;
	}
	if (kar_token_child(token, 3)->type != KAR_TOKEN_SIGN_COLON) {
		return false;
	}
	if (kar_token_child(token, 4)->type != KAR_TOKEN_BLOCK_BODY) {
		return false;
	}
	return true;
}

bool kar_parser_check_else(KarToken* token) {
	if (token->children.count != 3) {
		return false;
	}
	if (kar_token_child(token, 0)->type != KAR_TOKEN_COMMAND_ELSE) {
		return false;
	}
	if (kar_token_child(token, 1)->type != KAR_TOKEN_SIGN_COLON) {
		return false;
	}
	if (kar_token_child(token, 2)->type != KAR_TOKEN_BLOCK_BODY) {
		return false;
	}
	return true;
}

