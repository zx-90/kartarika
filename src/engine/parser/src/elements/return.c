/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool make_return(KToken* token) {
	if (token->type == TOKEN_INDENT && (token->children_count > 0) && token->children[0]->type == TOKEN_FUNC_RETURN) {
		size_t i;
		for (i = 1; i < token->children_count; ++i) {
			KToken* child = k_token_tear_child(token, 1);
			k_token_add_child(token->children[0], child);
		}
	}
	return true;
}

bool k_parser_make_return(KToken* token)
{
	return k_token_foreach(token, make_return);
}
