/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool remove_space(KarToken* token) {
	size_t i;
	for (i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		if ( (child->type == KAR_TOKEN_SPACE) || (child->type == KAR_TOKEN_INDENT && (child->children.count == 0) ) ) {
			kar_token_child_erase(token, i);
			i--;
		}
	}
	return true;
}

bool kar_parser_remove_spaces(KarToken* token)
{
	return kar_token_child_foreach_bool(token, remove_space);
}
