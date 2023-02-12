/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

static void prepare_return(KarToken* token) {
	token->type = KAR_TOKEN_COMMAND_RETURN;
	token->cursor = kar_token_child_get(token, 0)->cursor;
	kar_token_set_str(token, NULL);
	kar_token_child_erase(token, 0);
}

KarParserStatus kar_parser_make_return(KarToken* token, KarProjectErrorList* errors) {
	size_t returnPos = kar_token_child_find(token, KAR_TOKEN_COMMAND_RETURN);
	
	if (returnPos == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	KarToken* returnToken = kar_token_child_get(token, returnPos);
	if (returnPos != 0) {
		kar_project_error_list_create_add(errors, &returnToken->cursor, 1, "Ключевое слово вернуть должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	// Вернуть без параментра.
	if (kar_token_child_count(token) == 1) {
		prepare_return(token);
		return KAR_PARSER_STATUS_PARSED;
	}
	
	KarToken* expression = kar_token_child_get(token, 1);
	if (!kar_parser_is_expression(expression->type)) {
		kar_project_error_list_create_add(errors, &expression->cursor, 1, "Здесь ожидалось выражение.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) > 2) {
		kar_project_error_list_create_add(errors, &kar_token_child_get(token, 2)->cursor, 1, "Здесь ожидался конец команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	prepare_return(token);
	
	return KAR_PARSER_STATUS_PARSED;
}
