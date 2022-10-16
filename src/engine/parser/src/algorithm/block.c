/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_block(KarToken* token, KarArray* errors) {
	size_t blockPos = kar_token_child_find(token, KAR_TOKEN_COMMAND_BLOCK);
	
	if (blockPos == token->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	if (blockPos != 0) {
		kar_module_error_create_add(errors, &kar_token_child(token, blockPos)->cursor, 1, "Ключевое слово \"блок\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 2) {
		// TODO: Здесь должна быть последняя позиция в токене.
		kar_module_error_create_add(errors, &kar_token_child(token, blockPos)->cursor, 1, "Не возможно найти знак двоеточия после ключевого слова \"блок\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* colonToken = kar_token_child(token, 1);
	if (colonToken->type != KAR_TOKEN_SIGN_COLON) {
		kar_module_error_create_add(errors, &colonToken->cursor, 1, "Не возможно найти знак двоеточия после ключевого слова \"блок\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 3) {
		// TODO: Здесь должна быть последняя позиция в токене.
		kar_module_error_create_add(errors, &colonToken->cursor, 1, "Не возможно найти тело блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* bodyToken = kar_token_child(token, 2);
	if (bodyToken->type != KAR_TOKEN_BLOCK_BODY) {
		kar_module_error_create_add(errors, &bodyToken->cursor, 1, "Не возможно найти тело блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (token->children.count > 3) {
		kar_module_error_create_add(errors, &kar_token_child(token, 3)->cursor, 1, "Найдены лишние элементы после тела блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (!kar_parser_parse_algorithm(bodyToken, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = KAR_TOKEN_COMMAND_BLOCK;
	token->cursor = kar_token_child(token, blockPos)->cursor;
	kar_token_set_str(token, NULL);
	kar_token_child_erase(token, 0);
	kar_token_child_erase(token, 0);
	
	return KAR_PARSER_STATUS_PARSED;
}
