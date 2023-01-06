/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "structure/token.h"
#include "structure/module_error.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_if(KarToken* parent, size_t commandNum, KarArray* errors) {
	KarToken* token = kar_token_child(parent, commandNum);
	size_t ifPos = kar_token_child_find(token, KAR_TOKEN_COMMAND_IF);
	if (ifPos == token->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	KarToken* ifToken = kar_token_child(token, ifPos);
	if (ifPos != 0) {
		kar_module_error_create_add(errors, &ifToken->cursor, 1, "Ключевое слово \"если\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 2) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &ifToken->cursor, 1, "После ключевого слова \"если\" ожидалось условие (выражение, возвращающее булево значение).");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* ifExpressionToken = kar_token_child(token, 1);
	if (!kar_parser_is_expression(ifExpressionToken->type)) {
		kar_module_error_create_add(errors, &ifExpressionToken->cursor, 1, "Здесь ожидалось выражение, возвращающее булево значение.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 3) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &ifExpressionToken->cursor, 1, "Здесь ожидался знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* colonToken = kar_token_child(token, 2);
	if (colonToken->type != KAR_TOKEN_SIGN_COLON) {
		kar_module_error_create_add(errors, &colonToken->cursor, 1, "Здесь ожидался знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 4) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &colonToken->cursor, 1, "Не могу найти тело блока \"если\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* bodyToken = kar_token_child(token, 3);
	if (bodyToken->type != KAR_TOKEN_BLOCK_BODY) {
		kar_module_error_create_add(errors, &bodyToken->cursor, 1, "Здесь ожидалось найти тело блока \"если\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (token->children.count > 4) {
		kar_module_error_create_add(errors, &kar_token_child(token, 4)->cursor, 1, "Найдены лишние элементы после тела блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = ifToken->type;
	token->cursor = ifToken->cursor;
	kar_token_set_str(token, NULL);
	kar_token_child_erase(token, 0);
	kar_token_child_erase(token, 1);
	if (!kar_parser_parse_algorithm(bodyToken, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	while (parent->children.count > commandNum + 1) {
		KarToken* nextToken = kar_token_child(parent, commandNum + 1);
		if (kar_parser_check_ifelse(nextToken)) {
			KarToken* elseifExpressionToken = kar_token_child(nextToken, 2);
			KarToken* elseifBodyToken = kar_token_child(nextToken, 4);
			if (!kar_parser_parse_algorithm(elseifBodyToken, errors)) {
				return KAR_PARSER_STATUS_ERROR;
			}
			kar_token_child_tear(nextToken, 2);
			kar_token_child_tear(nextToken, 3);
			kar_token_child_erase(parent, commandNum + 1);
			kar_token_child_add(token, elseifExpressionToken);
			kar_token_child_add(token, elseifBodyToken);
		} else if (kar_parser_check_else(nextToken)) {
			KarToken* elseBodyToken = kar_token_child(nextToken, 2);
			if (!kar_parser_parse_algorithm(elseBodyToken, errors)) {
				return KAR_PARSER_STATUS_ERROR;
			}
			KarToken* elseExpressionToken = kar_token_create_fill(KAR_TOKEN_VAL_TRUE, kar_token_child(nextToken, 0)->cursor, NULL);
			kar_token_child_tear(nextToken, 2);
			kar_token_child_erase(parent, commandNum + 1);
			kar_token_child_add(token, elseExpressionToken);
			kar_token_child_add(token, elseBodyToken);
			break;
		} else {
			break;
		}
	}
	
	return KAR_PARSER_STATUS_PARSED;
}
