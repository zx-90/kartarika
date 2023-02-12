/* Copyright © 2020,2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

static bool make_path(KarToken* token, KarProjectErrorList* errors) {
	for (size_t i = kar_token_child_count(token); i > 0; --i) {
		size_t num = i - 1;
		KarToken* child = kar_token_child_get(token, num);
		if (child->type != KAR_TOKEN_SIGN_GET_FIELD) {
			continue;
		}
		
		if (num == 0) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Нет первого операнда у операции \".\".");
			return false;
		}
		if (num == kar_token_child_count(token) - 1) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Нет второго операнда у операции \"+\".");
			return false;
		}
		KarToken* first = kar_token_child_get(token, num - 1);
		KarToken* second = kar_token_child_get(token, num + 1);
		if (!kar_parser_is_expression(first->type)) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "У операции \".\" нет первого операнда или он не корректен.");
			return false;
		}
		if (!kar_parser_is_expression(second->type)) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "У операции \".\" нет второго операнда или он не корректен.");
			return false;
		}
		kar_token_child_tear(token, num + 1);
		kar_token_child_tear(token, num - 1);
		kar_token_child_add(child, first);
		kar_token_child_add(child, second);
		--i;
	}
	return true;
}

static bool foreach(KarToken* token, KarProjectErrorList* errors) 
{
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		if (!foreach(kar_token_child_get(token, i), errors)) {
			return false;
		}
	}
	return make_path(token, errors);
}

bool kar_parser_make_path(KarToken* token, KarProjectErrorList* errors)
{
	return foreach(token, errors);
}
