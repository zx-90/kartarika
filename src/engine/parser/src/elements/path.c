/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool make_path(KToken* token) {
	// TODO: Скобка может обозначать ещё рассчёт индекса массива, типа Товары.(3 + 4). Надо учитывать такой случай.
	// Нахожение функции как "идентификатор + скобки" без пробелов между ними.
	if (token->children_count < 2) {
		return true;
	}
	size_t i;
	for (i = token->children_count - 1; i >= 1; --i) {
		if (token->children[i - 1]->type == TOKEN_IDENTIFIER && token->children[i]->type == TOKEN_SIGN_OPEN_BRACES) {
			KToken* child = k_token_tear_child(token, i);
			k_token_add_child(token->children[i - 1], child);
		}
	}
	// Нахождение пути как "идентификатор + точка + идентификатор" без пробелов между ними.
	if (token->children_count < 3) {
		return true;
	}
	for (i = token->children_count - 1; i >= 2; --i) {
		if (token->children[i - 1]->type == TOKEN_SIGN_GET_FIELD) {
			KToken* third = k_token_tear_child(token, i);
			KToken* second = k_token_tear_child(token, i - 1);
			k_token_add_child(second, third);
			k_token_add_child(token->children[i - 2], second);
			i -= 1;
		}
	}
	return true;
}

bool k_parser_make_path(KToken* token)
{
	return k_token_foreach(token, make_path);
}
