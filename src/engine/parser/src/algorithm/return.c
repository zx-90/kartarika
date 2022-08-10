/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool make_return(KarToken* token) {
	// TODO: Проверить 2 варианта: "вернуть" и "вернуть ВЫРАЖЕНИЕ".
	if (token->type == KAR_TOKEN_INDENT && (token->children.count > 0) && kar_token_child(token, 0)->type == KAR_TOKEN_METHOD_RETURN) {
		kar_token_child_move_to_end(token, kar_token_child(token, 0), 1, token->children.count - 1);
	}
	return true;
}

bool kar_parser_make_return(KarToken* token)
{
	return kar_token_child_foreach_bool(token, make_return);
}
