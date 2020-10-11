/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool make_path(KarToken* token) {
	// TODO: Скобка может обозначать ещё рассчёт индекса массива, типа Товары.(3 + 4). Надо учитывать такой случай.
	// Нахожение функции как "идентификатор + скобки" без пробелов между ними.
	if (token->children.count < 2) {
		return true;
	}
	size_t i;
	for (i = token->children.count - 1; i >= 1; --i) {
		if (kar_token_child(token, i - 1)->type == KAR_TOKEN_IDENTIFIER && kar_token_child(token, i)->type == KAR_TOKEN_SIGN_OPEN_BRACES) {
			KarToken* child = kar_token_child_tear(token, i);
			kar_token_child_add(kar_token_child(token, i - 1), child);
		}
	}
	// Нахождение пути как "идентификатор + точка + идентификатор" без пробелов между ними.
	if (token->children.count < 3) {
		return true;
	}
	for (i = token->children.count - 1; i >= 2; --i) {
		if (kar_token_child(token, i - 1)->type == KAR_TOKEN_SIGN_GET_FIELD) {
			KarToken* third = kar_token_child_tear(token, i);
			KarToken* second = kar_token_child_tear(token, i - 1);
			kar_token_child_add(second, third);
			kar_token_child_add(kar_token_child(token, i - 2), second);
			i -= 1;
		}
	}
	return true;
}

bool kar_parser_make_path(KarToken* token)
{
	return kar_token_child_foreach_bool(token, make_path);
}
