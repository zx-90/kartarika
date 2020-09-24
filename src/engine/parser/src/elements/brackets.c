/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>
#include <stdlib.h>

#include "core/token.h"

static bool extern_bracket(KToken* token) {
	size_t* opens = (size_t*)malloc(sizeof(size_t) * token->children_count);
	size_t opens_cursor = 0;
	for (size_t i = 0; i < token->children_count; ++i) {
		if (token->children[i]->type == TOKEN_SIGN_OPEN_BRACES) {
			opens[opens_cursor] = i;
			opens_cursor++;
		}
		if (token->children[i]->type == TOKEN_SIGN_CLOSE_BRACES) {
			if (opens_cursor == 0) {
				free(opens);
				return false;
			}
			opens_cursor--;
			size_t open = opens[opens_cursor];
			for (size_t j = open + 1; j < i; ++j) {
				KToken* child = k_token_tear_child(token, open + 1);
				k_token_add_child(token->children[open], child);
			}
			k_token_erase_child(token, open + 1);
			i = open + 1;
		}
	}
	free(opens);
	return opens_cursor == 0;
}

bool k_parser_extern_brackets(KToken* token)
{
	return k_token_foreach(token, extern_bracket);
}
