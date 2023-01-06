/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "structure/module_error.h"
#include "structure/token.h"

static bool is_empty_token(KarToken* token) {
	return token->type == KAR_TOKEN_SPACE || token->type == KAR_TOKEN_COMMENT;
}

static bool is_empty_line(KarToken* token) {
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		if (!is_empty_token(child)) {
			return false;
		}
	}
	return true;
}

bool kar_parser_split_by_lines(KarToken* token, KarArray* errors)
{
	if (token->children.count < 1 || kar_token_child(token, 0)->type != KAR_TOKEN_INDENT)
	{
		kar_module_error_create_add(errors, 0, 1, "Внутрення ошибка. В модуле нет ни одного токена.");
		return false;
	}
	
	size_t line = 0;
	while (line != token->children.count)
	{
		size_t next_line;
		for (next_line = line + 1; next_line < token->children.count; ++next_line) {
			if (kar_token_child(token, next_line)->type == KAR_TOKEN_INDENT) {
				break;
			}
		}
		kar_token_child_move_to_end(token, kar_token_child(token, line), line + 1, next_line - line - 1);
		line++;
	}
	
	return true;
}

bool kar_parser_erase_empty_lines(KarToken* token) {
	size_t i = 0;
	while (i < token->children.count) {
		KarToken* line = kar_token_child(token, i);
		if (is_empty_line(line)) {
			kar_token_child_erase(token, i);
		} else {
			i++;
		}
	}
	return true;
}
