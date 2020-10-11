/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/alloc.h"
#include "core/token.h"

static bool extern_bracket(KarToken* token) {
	KAR_CREATES(opens, size_t, token->children.count);
	size_t opens_cursor = 0;
	for (size_t i = 0; i < token->children.count; ++i) {
		if (kar_token_child(token, i)->type == KAR_TOKEN_SIGN_OPEN_BRACES) {
			opens[opens_cursor] = i;
			opens_cursor++;
		}
		if (kar_token_child(token, i)->type == KAR_TOKEN_SIGN_CLOSE_BRACES) {
			if (opens_cursor == 0) {
				KAR_FREE(opens);
				return false;
			}
			opens_cursor--;
			size_t open = opens[opens_cursor];
			kar_token_child_move_to_end(token, kar_token_child(token, open), open + 1, i - open - 1);
			kar_token_child_erase(token, open + 1);
			i = open + 1;
		}
	}
	KAR_FREE(opens);
	return opens_cursor == 0;
}

bool kar_parser_extern_brackets(KarToken* token)
{
	return kar_token_child_foreach_bool(token, extern_bracket);
}
