/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

static bool check_assign(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	if (kar_token_child_count(token) == 1) {
		// TODO: курсор в конце токена.
		kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, 0)->cursor, 1, "Отсутствует имя переменной.");
		return false;
	}
	KarToken* name = kar_token_child_get(token, 1);
	if (!kar_token_is_name(name->type)) {
		kar_project_error_list_create_add(errors, moduleName, &name->cursor, 1, "Не корректное имя переменной.");
		return false;
	}

	if (kar_token_child_count(token) == 2) {
		// TODO: курсор в конце токена.
		kar_project_error_list_create_add(errors, moduleName, &name->cursor, 1, "Отсутствует символ присвоения \"=\".");
		return false;
	}
	KarToken* assign = kar_token_child_get(token, 2);
	if (assign->type != KAR_TOKEN_SIGN_ASSIGN) {
		kar_project_error_list_create_add(errors, moduleName, &assign->cursor, 1, "Ожидался символ присвоения \"=\".");
		return false;
	}

	if (kar_token_child_count(token) == 3) {
		// TODO: курсор в конце токена.
		kar_project_error_list_create_add(errors, moduleName, &assign->cursor, 1, "Отсутствует правая часть присвоения.");
		return false;
	}
	KarToken* expression = kar_token_child_get(token, 3);
	if (!kar_parser_is_expression(expression->type)) {
		kar_project_error_list_create_add(errors, moduleName, &expression->cursor, 1, "Не корректное выражение.");
		return false;
	}

	if (kar_token_child_count(token) > 4) {
		// TODO: курсор в конце токена.
		kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, 4)->cursor, 1, "Здесь ожидался конец команды.");
		return false;
	}
	return true;
}

KarParserStatus kar_parser_make_var_declaration(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	size_t returnPos = kar_token_child_find(token, KAR_TOKEN_FIELD_VAR);

	if (returnPos == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	if (returnPos != 0) {
		kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, returnPos)->cursor, 1, "Ключевое слово \"поле\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}

	if (!check_assign(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}

	token->type = KAR_TOKEN_COMMAND_DECLARATION;
	kar_token_set_str(token, NULL);
	token->cursor = kar_token_child_get(token, 0)->cursor;

	KarToken* expression = kar_token_child_tear(token, 3);
	KarToken* name = kar_token_child_tear(token, 1);

	// TODO: удаление всех дочерних элементов токена. Написать отдельную функцию.
	kar_token_child_erase(token, 0);
	kar_token_child_erase(token, 0);

	kar_token_child_add(token, name);
	kar_token_child_add(token, expression);

	return KAR_PARSER_STATUS_PARSED;
}

KarParserStatus kar_parser_make_const_declaration(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	size_t returnPos = kar_token_child_find(token, KAR_TOKEN_FIELD_CONST);

	if (returnPos == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	if (returnPos != 0) {
		kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, returnPos)->cursor, 1, "Ключевое слово \"поле\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}

	if (!check_assign(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}

	token->type = KAR_TOKEN_COMMAND_CONST_DECLARATION;
	kar_token_set_str(token, NULL);
	token->cursor = kar_token_child_get(token, 0)->cursor;

	KarToken* expression = kar_token_child_tear(token, 3);
	KarToken* name = kar_token_child_tear(token, 1);

	// TODO: удаление всех дочерних элементов токена. Написать отдельную функцию.
	kar_token_child_erase(token, 0);
	kar_token_child_erase(token, 0);

	kar_token_child_add(token, name);
	kar_token_child_add(token, expression);

	return KAR_PARSER_STATUS_PARSED;
}
