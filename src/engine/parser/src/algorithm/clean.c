/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"
#include "parser/base.h"

static bool kar_check_else(KarToken* token) {
	if (token->children.count != 3) {
		return false;
	}
	if (kar_token_child(token, 0)->type != KAR_TOKEN_COMMAND_ELSE) {
		return false;
	}
	if (kar_token_child(token, 1)->type != KAR_TOKEN_SIGN_COLON) {
		return false;
	}
	if (kar_token_child(token, 2)->type != KAR_TOKEN_BLOCK_BODY) {
		return false;
	}
	return true;
}

KarParserStatus kar_parser_make_clean(KarToken* parent, size_t commandNum, KarArray* errors) {
	
	KarToken* token = kar_token_child(parent, commandNum);
	size_t cleanPos = kar_token_child_find(token, KAR_TOKEN_COMMAND_CLEAN);
	if (cleanPos == token->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	KarToken* cleanToken = kar_token_child(token, cleanPos);
	if (cleanPos != 0) {
		kar_module_error_create_add(errors, &cleanToken->cursor, 1, "Ключевое слово \"раскрыть\" должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 2) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &cleanToken->cursor, 1, "Ожидалось имя переменной после ключевого слова \"раскрыть\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* uncleanNameToken = kar_token_child(token, 1);
	if (!kar_parser_is_expression(uncleanNameToken->type)) {
		kar_module_error_create_add(errors, &uncleanNameToken->cursor, 1, "Здесь ожидалось имя раскрываемой переменной.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count < 3) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &uncleanNameToken->cursor, 1, "Здесь ожидалось ключевое слово \"как\" или знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	size_t colonNum = 2;
	KarToken* asToken = kar_token_child(token, 2);
	KarToken* cleanNameToken = NULL;
	if (asToken->type == KAR_TOKEN_COMMAND_AS) {
		if (token->children.count < 4) {
			// TODO: Ссылка на последний символ в токене.
			kar_module_error_create_add(errors, &asToken->cursor, 1, "Здесь ожидалось имя переменной для блока раскрытия.");
			return KAR_PARSER_STATUS_ERROR;
		}
		cleanNameToken = kar_token_child(token, 3);
		if (!kar_token_is_name(cleanNameToken->type)) {
			kar_module_error_create_add(errors, &cleanNameToken->cursor, 1, "Здесь ожидалось имя переменной для блока раскрытия.");
			return KAR_PARSER_STATUS_ERROR;
		}
		colonNum += 2;
	}
	
	if (token->children.count < colonNum + 1) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &kar_token_child(token, colonNum)->cursor, 1, "Здесь ожидалось ключевое слово \"как\" или знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* colonToken = kar_token_child(token, colonNum);
	if (colonToken->type != KAR_TOKEN_SIGN_COLON) {
		kar_module_error_create_add(errors, &colonToken->cursor, 1, "Здесь ожидалось ключевое слово \"как\" или знак двоеточия.");
		return KAR_PARSER_STATUS_ERROR;
	}
	colonNum++;
	
	if (token->children.count < colonNum + 1) {
		// TODO: Ссылка на последний символ в токене.
		kar_module_error_create_add(errors, &kar_token_child(token, colonNum - 1)->cursor, 1, "Не могу найти тело блока раскрыть.");
		return KAR_PARSER_STATUS_ERROR;
	}
	KarToken* bodyToken = kar_token_child(token, colonNum);
	if (bodyToken->type != KAR_TOKEN_BLOCK_BODY) {
		kar_module_error_create_add(errors, &bodyToken->cursor, 1, "Здесь ожидалось найти тело блока \"раскрыть\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (token->children.count > colonNum + 1) {
		kar_module_error_create_add(errors, &kar_token_child(token, colonNum + 1)->cursor, 1, "Найдены лишние элементы после тела блока.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (!kar_parser_parse_algorithm(bodyToken, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = cleanToken->type;
	token->cursor = cleanToken->cursor;
	kar_token_set_str(token, NULL);
	kar_token_child_erase(token, 0);
	if (cleanNameToken) {
		kar_token_child_erase(token, 1);
		kar_token_child_erase(token, 2);
	} else {
		cleanNameToken = kar_token_create_fill(KAR_TOKEN_VAL_NULL, colonToken->cursor, NULL);
		kar_token_child_insert(token, cleanNameToken, 2);
		kar_token_child_erase(token, 1);
	}
	
	if (parent->children.count <= commandNum + 1) {
		return KAR_PARSER_STATUS_PARSED;
	}
	KarToken* elseToken = kar_token_child(parent, commandNum + 1);
	if (!kar_check_else(elseToken)) {
		return KAR_PARSER_STATUS_PARSED;
	}
	KarToken* elseBody = kar_token_child_tear(elseToken, 2);
	kar_token_child_erase(parent, commandNum + 1);
	kar_token_child_add(token, elseBody);
	if (!kar_parser_parse_algorithm(elseBody, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	return KAR_PARSER_STATUS_PARSED;
}
