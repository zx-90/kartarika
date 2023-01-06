/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/module_error.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_assign(KarToken* token, KarArray* errors) {
	size_t returnPos = kar_token_child_find(token, KAR_TOKEN_SIGN_ASSIGN);
	
	if (returnPos == token->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	if (returnPos < 1) {
		kar_module_error_create_add(errors, &kar_token_child(token, returnPos)->cursor, 1, "Отсутствует левая часть присвоения.");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (returnPos > 1) {
		kar_module_error_create_add(errors, &kar_token_child(token, 1)->cursor, 1, "Лишние токены в левой части присвоения.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* assign = kar_token_child(token, 1);
	
	KarToken* name = kar_token_child(token, 0);
	if (!kar_parser_is_expression(name->type)) {
		kar_module_error_create_add(errors, &name->cursor, 1, "Не корректное имя переменной.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count == 2) {
		// TODO: курсор в конце токена.
		kar_module_error_create_add(errors, &assign->cursor, 1, "Отсутствует правая часть присвоения.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* expression = kar_token_child(token, 2);
	if (!kar_parser_is_expression(expression->type)) {
		kar_module_error_create_add(errors, &expression->cursor, 1, "Не корректное выражение.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count > 3) {
		// TODO: курсор в конце токена.
		kar_module_error_create_add(errors, &kar_token_child(token, 3)->cursor, 1, "Здесь ожидался конец команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = KAR_TOKEN_COMMAND_ASSIGN;
	token->cursor = kar_token_get_first_grandchild(token)->cursor;
	kar_token_set_str(token, NULL);
	kar_token_child_erase(token, 1);
	
	return KAR_PARSER_STATUS_PARSED;
}
