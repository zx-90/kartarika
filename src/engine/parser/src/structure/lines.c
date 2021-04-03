/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/module_error.h"
#include "core/token.h"

bool kar_parser_split_by_lines(KarToken* token, KarArray* errors)
{
	if (token->children.count < 1 || kar_token_child(token, 0)->type != KAR_TOKEN_INDENT)
	{
		kar_module_error_create_add(errors, 0, 1, "Внутрення ошибка. В модуле нет ни одного токена.");
		return false;
	}
	size_t cursor = 0;
	while (cursor != token->children.count)
	{
		size_t j;
		for (j = cursor + 1; j < token->children.count; ++j) {
			if (kar_token_child(token, j)->type == KAR_TOKEN_INDENT) {
				break;
			}
		}
		kar_token_child_move_to_end(token, kar_token_child(token, cursor), cursor + 1, j - cursor - 1);
		cursor++;
	}
	return true;
}
