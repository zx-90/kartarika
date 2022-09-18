/* Copyright © 2021,2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"
#include "core/module_error.h"

static bool make_call_method(KarToken* token) {
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
		
		
		
		child->type = KAR_TOKEN_SIGN_CALL_METHOD;
		child->cursor = prev->cursor;
		kar_token_child_tear(token, i - 1);
		kar_token_child_insert(child, prev, 0);
		
	}
	return true;
}

static bool foreach(KarToken* token) 
{
	for (size_t i = 0; i < token->children.count; i++) {
		if (!foreach(token->children.items[i])) {
			return false;
		}
	}
	return make_call_method(token);
}

bool kar_parser_make_call_method(KarToken* token)
{
	return foreach(token);
}


// ----------------------------------------------------------------------------
// Обработка вызова функции.
// ----------------------------------------------------------------------------

static bool make_arguments(KarToken* token, KarArray* errors) {
	if (token->type != KAR_TOKEN_SIGN_CALL_METHOD) {
		return true;
	}
	if (token->children.count <= 1) {
		return true;
	}
	
	KarToken* currentArg = NULL;
	size_t currentPlace = 1;
	while (token->children.count > currentPlace) {
		KarToken* curItem = kar_token_child_tear(token, currentPlace);
		if (curItem->type == KAR_TOKEN_SIGN_COMMA) {
			if (currentArg == NULL) {
				kar_module_error_create_add(errors, &curItem->cursor, 1, "Неожиданное появление знака \",\".");
				kar_token_free(curItem);
				return false;
			}
			if (token->children.count == currentPlace) {
				kar_module_error_create_add(errors, &curItem->cursor, 1, "Нет операнда слева у знака \",\".");
				kar_token_free(curItem);
				return false;
			}
			kar_token_free(curItem);
			currentArg = NULL;
			continue;
		}
		if (currentArg == NULL) {
			currentArg = kar_token_create();
			currentArg->type = KAR_TOKEN_SIGN_ARGUMENT;
			currentArg->cursor = curItem->cursor;
			kar_token_child_insert(token, currentArg, currentPlace);
			currentPlace++;
		}
		kar_token_child_add(currentArg, curItem);
	}
	
	return true;
}

bool kar_parser_make_arguments(KarToken* token, KarArray* errors) 
{
	for (size_t i = 0; i < token->children.count; i++) {
		if (!kar_parser_make_arguments(token->children.items[i], errors)) {
			return false;
		}
	}
	return make_arguments(token, errors);
}
