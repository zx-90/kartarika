/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/second_lexer.h"

#include <string.h>

#include "lexer/keyword.h"

static void retype_if_check(KarToken* token, KarTokenType checkType, const char* str, KarTokenType newType) {
	if (token->type == checkType && !strcmp(token->str, str)) {
		token->type = newType;
	}
}

static bool is_cypher(const char c) {
	return (c >= 0x30 && c <= 0x39);
}

static bool check_for_number(KarToken* token, KarModule* module) {
	if (token->type != KAR_TOKEN_IDENTIFIER) {
		return true;
	}
	if (!is_cypher(token->str[0])) {
		return true;
	}
	size_t len = strlen(token->str);
	for (size_t i = 1; i < len; ++i) {
		if(!is_cypher(token->str[i])) {
			kar_module_error_create_add(&module->errors, &token->cursor, 1, "Токен не является корректным числом.");
			return false;
		}
	}
	token->type = KAR_TOKEN_VAL_INTEGER;
	return true;
}

static bool retype_all(KarToken* token, KarModule* module) {
	// Значения переменных.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_NULL, KAR_TOKEN_VAL_NULL);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_TRUE, KAR_TOKEN_VAL_TRUE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_FALSE, KAR_TOKEN_VAL_FALSE);
	
	// Число.
	if (!check_for_number(token, module)) {
		return false;
	}
	
	// Объявления переменных.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_DECLARATION_CONST, KAR_TOKEN_FIELD_CONST);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_DECLARATION_VAR, KAR_TOKEN_FIELD_VAR);
	
	// Модификаторы поля
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAR_MODIFIER_DYNAMIC, KAR_TOKEN_VAR_MODIFIER_DYNAMIC);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAR_MODIFIER_PRIVATE, KAR_TOKEN_VAR_MODIFIER_PRIVATE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAR_MODIFIER_PROTECTED, KAR_TOKEN_VAR_MODIFIER_PROTECTED);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAR_MODIFIER_PUBLIC, KAR_TOKEN_VAR_MODIFIER_PUBLIC);
	
	// Типы переменных.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_BOOL, KAR_TOKEN_VAR_BOOL);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_INTEGER, KAR_TOKEN_VAR_INTEGER64);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_INTEGER8, KAR_TOKEN_VAR_INTEGER8);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_INTEGER16, KAR_TOKEN_VAR_INTEGER16);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_INTEGER32, KAR_TOKEN_VAR_INTEGER32);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_INTEGER64, KAR_TOKEN_VAR_INTEGER64);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_UNSIGNED, KAR_TOKEN_VAR_UNSIGNED64);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_UNSIGNED8, KAR_TOKEN_VAR_UNSIGNED8);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_UNSIGNED16, KAR_TOKEN_VAR_UNSIGNED16);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_UNSIGNED32, KAR_TOKEN_VAR_UNSIGNED32);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_UNSIGNED64, KAR_TOKEN_VAR_UNSIGNED64);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_FLOAT, KAR_TOKEN_VAR_FLOAT64);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_FLOAT32, KAR_TOKEN_VAR_FLOAT32);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_FLOAT64, KAR_TOKEN_VAR_FLOAT64);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_FLOAT80, KAR_TOKEN_VAR_FLOAT80);

	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_STRING, KAR_TOKEN_VAR_STRING);
	
	// Составные переменные.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_ENUMERATION, KAR_TOKEN_ENUMERATION);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_STRUCTURE, KAR_TOKEN_STRUCTURE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TYPE_UNION, KAR_TOKEN_UNION);
	
	// Знаки
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_OPEN_BRACES, KAR_TOKEN_SIGN_OPEN_BRACES);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_CLOSE_BRACES, KAR_TOKEN_SIGN_CLOSE_BRACES);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_NULLABLE, KAR_TOKEN_SIGN_NULLABLE);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_GET_FIELD, KAR_TOKEN_SIGN_GET_FIELD);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_COMMA, KAR_TOKEN_SIGN_COMMA);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_COLON, KAR_TOKEN_SIGN_COLON);
	
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_ASSIGN, KAR_TOKEN_SIGN_ASSIGN);
	
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_PLUS, KAR_TOKEN_SIGN_PLUS);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_MINUS, KAR_TOKEN_SIGN_MINUS);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_MUL, KAR_TOKEN_SIGN_MUL);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_DIV, KAR_TOKEN_SIGN_DIV);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_MOD, KAR_TOKEN_SIGN_MOD);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_AND, KAR_TOKEN_SIGN_AND);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_OR, KAR_TOKEN_SIGN_OR);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_NOT, KAR_TOKEN_SIGN_NOT);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_EQUAL, KAR_TOKEN_SIGN_EQUAL);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_GREATER, KAR_TOKEN_SIGN_GREATER);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_LESS, KAR_TOKEN_SIGN_LESS);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_AND, KAR_TOKEN_SIGN_BIT_AND);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_OR, KAR_TOKEN_SIGN_BIT_OR);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_NOT, KAR_TOKEN_SIGN_BIT_NOT);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_XOR, KAR_TOKEN_SIGN_BIT_XOR);
	
	// Управление последовательностью выполенения.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_BLOCK, KAR_TOKEN_COMMAND_BLOCK);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_PASS, KAR_TOKEN_COMMAND_PASS);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_IF, KAR_TOKEN_COMMAND_IF);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMANT_THEN, KAR_TOKEN_COMMANT_THEN);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_ELSE, KAR_TOKEN_COMMAND_ELSE);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_FOR, KAR_TOKEN_COMMAND_FOR);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_FROM, KAR_TOKEN_COMMAND_FROM);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_TO, KAR_TOKEN_COMMAND_TO);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_STEP, KAR_TOKEN_COMMAND_STEP);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_FOREACH, KAR_TOKEN_COMMAND_FOREACH);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_GET, KAR_TOKEN_COMMAND_GET);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_WHILE, KAR_TOKEN_COMMAND_WHILE);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_CONTINUE, KAR_TOKEN_COMMAND_CONTINUE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_BREAK, KAR_TOKEN_COMMAND_BREAK);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_COMMAND_OPEN, KAR_TOKEN_COMMAND_OPEN);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_DECLARATION_METHOD, KAR_TOKEN_METHOD);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_STAT, KAR_TOKEN_MODIFIER_STAT);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_METHOD_MODIFIER_DYNAMIC, KAR_TOKEN_METHOD_MODIFIER_DYNAMIC);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_METHOD_MODIFIER_PRIVATE, KAR_TOKEN_METHOD_MODIFIER_PRIVATE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_METHOD_MODIFIER_PROTECTED, KAR_TOKEN_METHOD_MODIFIER_PROTECTED);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_METHOD_MODIFIER_PUBLIC, KAR_TOKEN_METHOD_MODIFIER_PUBLIC);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_METHOD_MODIFIER_FINALIZED, KAR_TOKEN_METHOD_MODIFIER_FINALIZED);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_METHOD_MODIFIER_INHERITED, KAR_TOKEN_METHOD_MODIFIER_INHERITED);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_METHOD_MODIFIER_OVERLOAD, KAR_TOKEN_METHOD_MODIFIER_OVERLOAD);

	//KAR_TOKEN_METHOD_ARGUMENT_CONST,
	//KAR_TOKEN_METHOD_ARGUMENT_VAR,
	//KAR_TOKEN_METHOD_ARGUMENT_POINTER,
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_METHOD_RETURN, KAR_TOKEN_METHOD_RETURN);
	
	// Класс
	
	// Шаблон класса
	
	// Тест
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TEST, KAR_TOKEN_TEST);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_TEST_CHECK, KAR_TOKEN_CHECK);
	
	return true;
}

static bool foreach_retype(KarToken* token, KarModule* module) {
	for (size_t i = 0; i < token->children.count; i++) {
		if (!foreach_retype(token->children.items[i], module)) {
			return false;
		}
	}
	return retype_all(token, module);
}

bool kar_second_lexer_run(KarModule* module) {
	return foreach_retype(module->token, module);
}
