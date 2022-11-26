/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool concat_single_minus_number(KarToken* token) {
	if (token->type != KAR_TOKEN_SIGN_SINGLE_MINUS) {
		return true;
	}
	
	KarToken* child = kar_token_child(token, 0);
	if (child->type == KAR_TOKEN_VAL_INTEGER ||
		child->type == KAR_TOKEN_VAL_FLOAT
	) {
		token->type = KAR_TOKEN_VAL_INTEGER;
		kar_token_add_str(token, child->str);
		kar_token_child_erase(token, 0);
	}
	
	return true;
}

bool kar_parser_concat_single_minus_number(KarToken* token)
{
	return kar_token_child_foreach_bool(token, concat_single_minus_number);
}
