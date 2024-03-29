/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_block(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	size_t blockPos = kar_token_child_find(token, KAR_TOKEN_COMMAND_BLOCK);
	
	if (blockPos == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	if (blockPos != 0) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, blockPos)->cursor, 1, "Ключевое слово \"блок\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) < 2) {
		// TODO: Здесь должна быть последняя позиция в токене.
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, blockPos)->cursor, 1, "Не возможно найти знак двоеточия после ключевого слова \"блок\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* colonToken = kar_token_child_get(token, 1);
	if (colonToken->type != KAR_TOKEN_SIGN_COLON) {
        kar_project_error_list_create_add(errors, moduleName, &colonToken->cursor, 1, "Не возможно найти знак двоеточия после ключевого слова \"блок\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (kar_token_child_count(token) < 3) {
		// TODO: Здесь должна быть последняя позиция в токене.
        kar_project_error_list_create_add(errors, moduleName, &colonToken->cursor, 1, "Не возможно найти тело блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* bodyToken = kar_token_child_get(token, 2);
	if (bodyToken->type != KAR_TOKEN_BLOCK_BODY) {
        kar_project_error_list_create_add(errors, moduleName, &bodyToken->cursor, 1, "Не возможно найти тело блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (kar_token_child_count(token) > 3) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, 3)->cursor, 1, "Найдены лишние элементы после тела блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!kar_parser_parse_algorithm(bodyToken, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = KAR_TOKEN_COMMAND_BLOCK;
	token->cursor = kar_token_child_get(token, blockPos)->cursor;
	kar_token_set_str(token, NULL);
	kar_token_child_erase(token, 0);
	kar_token_child_erase(token, 0);
	
	return KAR_PARSER_STATUS_PARSED;
}
