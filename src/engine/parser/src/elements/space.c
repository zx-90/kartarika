/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool remove_space(KarToken* token) {
	size_t i;
	for (i = 0; i < token->children_count; ++i) {
		if ( (token->children[i]->type == KAR_TOKEN_SPACE) || (token->children[i]->type == KAR_TOKEN_INDENT && (token->children[i]->children_count == 0) ) ) {
			kar_token_erase_child(token, i);
			i--;
		}
	}
	return true;
}

bool kar_parser_remove_spaces(KarToken* token)
{
	return kar_token_foreach(token, remove_space);
}
