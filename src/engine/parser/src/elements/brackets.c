/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/alloc.h"
#include "core/module_error.h"
#include "core/token.h"

static bool extern_bracket(KarToken* token, KarArray* errors) {
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
				kar_module_error_create_add(errors, &kar_token_child(token, i)->cursor, 1, "У закрывающейся скобки нет соответствующей ей открывающейся.");
				return false;
			}
			opens_cursor--;
			size_t open = opens[opens_cursor];
			kar_token_child_move_to_end(token, kar_token_child(token, open), open + 1, i - open - 1);
			kar_token_child_erase(token, open + 1);
			i = open + 1;
		}
	}
	if (opens_cursor != 0) {
		kar_module_error_create_add(errors, &kar_token_child(token, opens[opens_cursor - 1])->cursor, 1, "У открывающейся скобки нет соответствующей ей закрывающейся.");
		KAR_FREE(opens);
		return false;
	}
	KAR_FREE(opens);
	return true;
}

static bool foreach(KarToken* token, KarArray* errors) 
{
	for (size_t i = 0; i < token->children.count; i++) {
		if (!foreach(token->children.items[i], errors)) {
			return false;
		}
	}
	return extern_bracket(token, errors);
}

bool kar_parser_extern_brackets(KarToken* token, KarArray* errors)
{
	return foreach(token, errors);
}
