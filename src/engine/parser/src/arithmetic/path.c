/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"
#include "core/module_error.h"

static bool make_call_func(KarToken* token) {
	if (token->children.count == 0) {
		return true;
	}

	for (size_t i = token->children.count - 1; i >= 1; --i) {
		KarToken* child = kar_token_child(token, i);
		if (child->type != KAR_TOKEN_SIGN_OPEN_BRACES) {
			continue;
		}
		
		KarToken* prev = kar_token_child(token, i - 1);
		if (prev->type != KAR_TOKEN_IDENTIFIER) {
			continue;
		}
		
		kar_token_child_tear(token, i);
		kar_token_child_add(prev, child);
	}
	return true;
}

static bool make_path(KarToken* token, KarArray* errors) {
	for (size_t i = token->children.count; i > 0; --i) {
		size_t num = i - 1;
		KarToken* child = kar_token_child(token, num);
		if (child->type != KAR_TOKEN_SIGN_GET_FIELD) {
			continue;
		}
		
		if (num == 0) {
			kar_module_error_create_add(errors, &child->cursor, 1, "Нет первого операнда у операции \".\".");
			return false;
		}
		if (num == token->children.count - 1) {
			kar_module_error_create_add(errors, &child->cursor, 1, "Нет второго операнда у операции \"+\".");
			return false;
		}
		KarToken* first = kar_token_child(token, num - 1);
		KarToken* second = kar_token_child(token, num + 1);
		if (first->type != KAR_TOKEN_IDENTIFIER && first->type != KAR_TOKEN_SIGN_OPEN_BRACES) {
			kar_module_error_create_add(errors, &child->cursor, 1, "У операции \".\" нет первого операнда или он не корректен.");
			return false;
		}
		if (second->type != KAR_TOKEN_IDENTIFIER && second->type != KAR_TOKEN_SIGN_OPEN_BRACES) {
			kar_module_error_create_add(errors, &child->cursor, 1, "У операции \".\" нет второго операнда или он не корректен.");
			return false;
		}
		kar_token_child_tear(token, num + 1);
		kar_token_child_tear(token, num);
		kar_token_child_add(first, child);
		kar_token_child_add(child, second);
		--i;
	}
	return true;
}

static bool foreach(KarToken* token, KarArray* errors) 
{
	for (size_t i = 0; i < token->children.count; i++) {
		if (!foreach(token->children.items[i], errors)) {
			return false;
		}
	}
	return make_call_func(token) && make_path(token, errors);
}

bool kar_parser_make_path(KarToken* token, KarArray* errors)
{
	return foreach(token, errors);
}
