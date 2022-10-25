/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_while(KarToken* token, KarArray* errors) {
	size_t whilePos = kar_token_child_find(token, KAR_TOKEN_COMMAND_WHILE);
	if (whilePos == token->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	KarToken* whileToken = kar_token_child(token, whilePos);
	if (whilePos != 0) {
		kar_module_error_create_add(errors, &whileToken->cursor, 1, "Ключевое слово \"пока\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 2) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &whileToken->cursor, 1, "После ключевого слова \"пока\" ожидалось условие (выражение, возвращающее булево значение).");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* whileExpressionToken = kar_token_child(token, 1);
	if (!kar_parser_is_expression(whileExpressionToken->type)) {
		kar_module_error_create_add(errors, &whileExpressionToken->cursor, 1, "Здесь ожидалось выражение, возвращающее булево значение.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 3) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &whileExpressionToken->cursor, 1, "Здесь ожидался знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* colonToken = kar_token_child(token, 2);
	if (colonToken->type != KAR_TOKEN_SIGN_COLON) {
		kar_module_error_create_add(errors, &colonToken->cursor, 1, "Здесь ожидался знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 4) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &colonToken->cursor, 1, "Не могу найти тело блока \"пока\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* bodyToken = kar_token_child(token, 3);
	if (bodyToken->type != KAR_TOKEN_BLOCK_BODY) {
		kar_module_error_create_add(errors, &bodyToken->cursor, 1, "Здесь ожидалось найти тело блока \"пока\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (token->children.count > 4) {
		kar_module_error_create_add(errors, &kar_token_child(token, 4)->cursor, 1, "Найдены лишние элементы после тела блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = whileToken->type;
	token->cursor = whileToken->cursor;
	kar_token_set_str(token, NULL);
	kar_token_child_erase(token, 0);
	kar_token_child_erase(token, 1);
	if (!kar_parser_parse_algorithm(bodyToken, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	return KAR_PARSER_STATUS_PARSED;
}
