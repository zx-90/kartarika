/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_while(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	size_t whilePos = kar_token_child_find(token, KAR_TOKEN_COMMAND_WHILE);
	if (whilePos == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	KarToken* whileToken = kar_token_child_get(token, whilePos);
	if (whilePos != 0) {
        kar_project_error_list_create_add(errors, moduleName, &whileToken->cursor, 1, "Ключевое слово \"пока\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) < 2) {
		// TODO: Ссылка на последний символ в токене.
        kar_project_error_list_create_add(errors, moduleName, &whileToken->cursor, 1, "После ключевого слова \"пока\" ожидалось условие (выражение, возвращающее булево значение).");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* whileExpressionToken = kar_token_child_get(token, 1);
	if (!kar_parser_is_expression(whileExpressionToken->type)) {
        kar_project_error_list_create_add(errors, moduleName, &whileExpressionToken->cursor, 1, "Здесь ожидалось выражение, возвращающее булево значение.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) < 3) {
		// TODO: Ссылка на последний символ в токене.
        kar_project_error_list_create_add(errors, moduleName, &whileExpressionToken->cursor, 1, "Здесь ожидался знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* colonToken = kar_token_child_get(token, 2);
	if (colonToken->type != KAR_TOKEN_SIGN_COLON) {
        kar_project_error_list_create_add(errors, moduleName, &colonToken->cursor, 1, "Здесь ожидался знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) < 4) {
		// TODO: Ссылка на последний символ в токене.
        kar_project_error_list_create_add(errors, moduleName, &colonToken->cursor, 1, "Не могу найти тело блока \"пока\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* bodyToken = kar_token_child_get(token, 3);
	if (bodyToken->type != KAR_TOKEN_BLOCK_BODY) {
        kar_project_error_list_create_add(errors, moduleName, &bodyToken->cursor, 1, "Здесь ожидалось найти тело блока \"пока\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (kar_token_child_count(token) > 4) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, 4)->cursor, 1, "Найдены лишние элементы после тела блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = whileToken->type;
	token->cursor = whileToken->cursor;
	kar_token_set_str(token, NULL);
	kar_token_child_erase(token, 0);
	kar_token_child_erase(token, 1);
    if (!kar_parser_parse_algorithm(bodyToken, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	return KAR_PARSER_STATUS_PARSED;
}
