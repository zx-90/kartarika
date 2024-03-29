/* Copyright © 2020,2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "model/token.h"

static bool is_space_token(KarToken* token) {
	return (token->type == KAR_TOKEN_SPACE) ||
			(token->type == KAR_TOKEN_COMMENT) ||
			(token->type == KAR_TOKEN_INDENT && (kar_token_child_empty(token)));
}

static bool remove_space(KarToken* token) {
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (is_space_token(child))
		{
			kar_token_child_erase(token, i);
			i--;
		} else {
			remove_space(child);
		}
	}
	return true;
}

bool kar_parser_remove_spaces(KarToken* token)
{
	return kar_token_child_foreach_bool(token, remove_space);
}
