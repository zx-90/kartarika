/* Copyright © 2021-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "model/token.h"
#include "model/project_error_list.h"

static bool is_method_operator(KarTokenType type) {
	return
		kar_token_type_is_identifier(type) ||
		kar_token_type_is_variable(type);
}

static bool make_call_method(KarToken* token) {
	if (kar_token_child_empty(token)) {
		return true;
	}

	for (size_t i = kar_token_child_count(token) - 1; i >= 1; --i) {
		KarToken* child = kar_token_child_get(token, i);
		if (child->type != KAR_TOKEN_SIGN_OPEN_BRACES) {
			continue;
		}
		
		KarToken* prev = kar_token_child_get(token, i - 1);
		if (!is_method_operator(prev->type)) {
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
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		if (!foreach(kar_token_child_get(token, i))) {
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

static bool make_arguments(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_SIGN_CALL_METHOD) {
		return true;
	}
	if (kar_token_child_count(token) <= 1) {
		return true;
	}
	
	KarToken* currentArg = NULL;
	size_t currentPlace = 1;
	while (kar_token_child_count(token) > currentPlace) {
		KarToken* curItem = kar_token_child_tear(token, currentPlace);
		if (curItem->type == KAR_TOKEN_SIGN_COMMA) {
			if (currentArg == NULL) {
                kar_project_error_list_create_add(errors, moduleName, &curItem->cursor, 1, "Неожиданное появление знака \",\".");
				kar_token_free(curItem);
				return false;
			}
			if (kar_token_child_count(token) == currentPlace) {
                kar_project_error_list_create_add(errors, moduleName, &curItem->cursor, 1, "Нет операнда слева у знака \",\".");
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

bool kar_parser_make_arguments(KarToken* token, KarString* moduleName, KarProjectErrorList* errors)
{
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
        if (!kar_parser_make_arguments(kar_token_child_get(token, i), moduleName, errors)) {
			return false;
		}
	}
    return make_arguments(token, moduleName, errors);
}
