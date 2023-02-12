/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_declaration(KarToken* token, KarProjectErrorList* errors) {
	size_t returnPos = kar_token_child_find(token, KAR_TOKEN_FIELD_VAR);
	
	if (returnPos == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	if (returnPos != 0) {
		kar_project_error_list_create_add(errors, &kar_token_child_get(token, returnPos)->cursor, 1, "Ключевое слово \"поле\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) == 1) {
		// TODO: курсор в конце токена.
		kar_project_error_list_create_add(errors, &kar_token_child_get(token, 0)->cursor, 1, "Отсутствует имя переменной.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* name = kar_token_child_get(token, 1);
	if (!kar_token_is_name(name->type)) {
		kar_project_error_list_create_add(errors, &name->cursor, 1, "Не корректное имя переменной.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) == 2) {
		// TODO: курсор в конце токена.
		kar_project_error_list_create_add(errors, &name->cursor, 1, "Отсутствует символ присвоения \"=\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* assign = kar_token_child_get(token, 2);
	if (assign->type != KAR_TOKEN_SIGN_ASSIGN) {
		kar_project_error_list_create_add(errors, &assign->cursor, 1, "Ожидался символ присвоения \"=\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) == 3) {
		// TODO: курсор в конце токена.
		kar_project_error_list_create_add(errors, &assign->cursor, 1, "Отсутствует правая часть присвоения.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* expression = kar_token_child_get(token, 3);
	if (!kar_parser_is_expression(expression->type)) {
		kar_project_error_list_create_add(errors, &expression->cursor, 1, "Не корректное выражение.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) > 4) {
		// TODO: курсор в конце токена.
		kar_project_error_list_create_add(errors, &kar_token_child_get(token, 4)->cursor, 1, "Здесь ожидался конец команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = KAR_TOKEN_COMMAND_DECLARATION;
	kar_token_set_str(token, NULL);
	token->cursor = kar_token_child_get(token, 0)->cursor;

	kar_token_child_tear(token, 3);
	kar_token_child_tear(token, 1);
	
	// TODO: удаление всех дочерних элементов токена. Написать отдельную функцию.
	kar_token_child_erase(token, 0);
	kar_token_child_erase(token, 0);
	
	kar_token_child_add(token, name);
	kar_token_child_add(token, expression);
	
	return KAR_PARSER_STATUS_PARSED;
}
