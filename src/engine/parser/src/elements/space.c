/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool remove_space(KToken* token) {
	size_t i;
	for (i = 0; i < token->children_count; ++i) {
		if ( (token->children[i]->type == TOKEN_SPACE) || (token->children[i]->type == TOKEN_INDENT && (token->children[i]->children_count == 0) ) ) {
			k_token_erase_child(token, i);
			i--;
		}
	}
	return true;
}

bool k_parser_remove_spaces(KToken* token)
{
	return k_token_foreach(token, remove_space);
}
