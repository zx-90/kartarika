/* Copyright © 2020,2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

static bool is_method_operator(KarTokenType type) {
	return
		type == KAR_TOKEN_SIGN_GET_FIELD ||
		kar_token_type_is_identifier(type) ||
		kar_token_type_is_variable(type);
}

static bool make_path_call(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);

		if (child->type == KAR_TOKEN_SIGN_GET_FIELD) {
		
			if (i == 0) {
				kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Нет первого операнда у операции \".\".");
				return false;
			}
			if (i == kar_token_child_count(token) - 1) {
				kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Нет второго операнда у операции \"+\".");
				return false;
			}
			KarToken* first = kar_token_child_get(token, i - 1);
			KarToken* second = kar_token_child_get(token, i + 1);
			if (!kar_parser_is_expression(first->type)) {
				kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "У операции \".\" нет первого операнда или он не корректен.");
				return false;
			}
			if (!kar_parser_is_expression(second->type)) {
				kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "У операции \".\" нет второго операнда или он не корректен.");
				return false;
			}
			kar_token_child_tear(token, i + 1);
			kar_token_child_tear(token, i - 1);
			kar_token_child_add(child, first);
			kar_token_child_add(child, second);
			i--;
		} else if (child->type == KAR_TOKEN_SIGN_OPEN_BRACES) {
			if (i == 0) {
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
			i--;
		}
	}
	return true;
}

static bool foreach(KarToken* token, KarString* moduleName, KarProjectErrorList* errors)
{
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
        if (!foreach(kar_token_child_get(token, i), moduleName, errors)) {
			return false;
		}
	}
	return make_path_call(token, moduleName, errors);
}

bool kar_parser_make_path_call(KarToken* token, KarString* moduleName, KarProjectErrorList* errors)
{
    return foreach(token, moduleName, errors);
}
