/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token_type.h"

const char* kar_token_type_get_name(const KarTokenType type)
{
#define PROCESS_VAL(p) case(KAR_TOKEN_##p): return #p;
	switch(type){
		PROCESS_VAL(UNKNOWN);
		// Первоначальные
		PROCESS_VAL(MODULE);
		PROCESS_VAL(COMMENT);
		PROCESS_VAL(INDENT);
		PROCESS_VAL(SPACE);
		PROCESS_VAL(IDENTIFIER);
		PROCESS_VAL(SIGN);
		PROCESS_VAL(VAL_STRING);
		
		// Значения переменых.
		PROCESS_VAL(VAL_NULL);
		PROCESS_VAL(VAL_TRUE);
		PROCESS_VAL(VAL_FALSE);
		PROCESS_VAL(VAL_INTEGER);
		PROCESS_VAL(VAL_HEXADECIMAL);
		PROCESS_VAL(VAL_FLOAT);
		PROCESS_VAL(VAL_NAN);
		PROCESS_VAL(VAL_INFINITY);
		PROCESS_VAL(VAL_MINUS_INFINITY);
		
		// Поля
		PROCESS_VAL(FIELD_CONST);
		PROCESS_VAL(FIELD_VAR);
		
		// Модификаторы поля
		PROCESS_VAL(VAR_MODIFIER);
		PROCESS_VAL(VAR_MODIFIER_DYNAMIC);
		PROCESS_VAL(VAR_MODIFIER_PRIVATE);
		PROCESS_VAL(VAR_MODIFIER_PROTECTED);
		PROCESS_VAL(VAR_MODIFIER_PUBLIC);
		
		// Простые переменные
		PROCESS_VAL(VAR_BOOL);
		
		PROCESS_VAL(VAR_INTEGER8);
		PROCESS_VAL(VAR_INTEGER16);
		PROCESS_VAL(VAR_INTEGER32);
		PROCESS_VAL(VAR_INTEGER64);
		
		PROCESS_VAL(VAR_UNSIGNED8);
		PROCESS_VAL(VAR_UNSIGNED16);
		PROCESS_VAL(VAR_UNSIGNED32);
		PROCESS_VAL(VAR_UNSIGNED64);
		
		PROCESS_VAL(VAR_FLOAT32);
		PROCESS_VAL(VAR_FLOAT64);
		PROCESS_VAL(VAR_FLOAT80);
		
		PROCESS_VAL(VAR_STRING);
		
		// Знаки
		PROCESS_VAL(SIGN_OPEN_BRACES);
		PROCESS_VAL(SIGN_CLOSE_BRACES);
		
		PROCESS_VAL(SIGN_GET_FIELD);
		PROCESS_VAL(SIGN_CALL_METHOD);
		PROCESS_VAL(SIGN_ARGUMENT);
		PROCESS_VAL(SIGN_UNCLEAN);
		PROCESS_VAL(SIGN_CLEAN);

		PROCESS_VAL(SIGN_COMMA);
		PROCESS_VAL(SIGN_COLON);
		
		PROCESS_VAL(SIGN_ASSIGN);
		
		PROCESS_VAL(SIGN_SINGLE_PLUS);
		PROCESS_VAL(SIGN_SINGLE_MINUS);
		PROCESS_VAL(SIGN_PLUS);
		PROCESS_VAL(SIGN_MINUS);
		PROCESS_VAL(SIGN_MUL);
		PROCESS_VAL(SIGN_DIV);
		PROCESS_VAL(SIGN_MOD);
		PROCESS_VAL(SIGN_DIV_CLEAN);
		PROCESS_VAL(SIGN_MOD_CLEAN);
		
		PROCESS_VAL(SIGN_AND);
		PROCESS_VAL(SIGN_OR);
		PROCESS_VAL(SIGN_NOT);
		PROCESS_VAL(SIGN_EQUAL);
		PROCESS_VAL(SIGN_NOT_EQUAL);
		PROCESS_VAL(SIGN_GREATER);
		PROCESS_VAL(SIGN_GREATER_OR_EQUAL);
		PROCESS_VAL(SIGN_LESS);
		PROCESS_VAL(SIGN_LESS_OR_EQUAL);
		
		PROCESS_VAL(SIGN_BIT_AND);
		PROCESS_VAL(SIGN_BIT_OR);
		PROCESS_VAL(SIGN_BIT_NOT);
		PROCESS_VAL(SIGN_BIT_XOR);
		PROCESS_VAL(SIGN_BIT_RIGHT);
		PROCESS_VAL(SIGN_BIT_LEFT);
		
		// Команды алгоритма.
		PROCESS_VAL(COMMAND_EXPRESSION);
		PROCESS_VAL(COMMAND_RETURN);
		PROCESS_VAL(COMMAND_DECLARATION);
		PROCESS_VAL(COMMAND_ASSIGN);
		
		PROCESS_VAL(COMMAND_BLOCK);
		PROCESS_VAL(COMMAND_PASS);
		
		PROCESS_VAL(COMMAND_CLEAN);
		PROCESS_VAL(COMMAND_AS);
		
		PROCESS_VAL(COMMAND_IF);
		PROCESS_VAL(COMMAND_ELSE);
		
		PROCESS_VAL(COMMAND_WHILE);
		PROCESS_VAL(COMMAND_CONTINUE);
		PROCESS_VAL(COMMAND_BREAK);
		
		PROCESS_VAL(BLOCK_BODY);
		
		// Функция.
		PROCESS_VAL(METHOD);
		PROCESS_VAL(METHOD_MODIFIER);
		PROCESS_VAL(MODIFIER_STAT);
		PROCESS_VAL(METHOD_MODIFIER_DYNAMIC);
		PROCESS_VAL(METHOD_MODIFIER_PRIVATE);
		PROCESS_VAL(METHOD_MODIFIER_PROTECTED);
		PROCESS_VAL(METHOD_MODIFIER_PUBLIC);
		PROCESS_VAL(METHOD_MODIFIER_FINALIZED);
		PROCESS_VAL(METHOD_MODIFIER_INHERITED);
		PROCESS_VAL(METHOD_MODIFIER_OVERLOAD);
		
		PROCESS_VAL(METHOD_PARAMETERS);
		PROCESS_VAL(METHOD_PARAMETER_CONST);
		PROCESS_VAL(METHOD_PARAMETER_VAR);
		
		PROCESS_VAL(METHOD_RETURN_TYPE);
	}
#undef PROCESS_VAL
	return "[UNKNOWN]";
}

bool kar_token_type_is_identifier(const KarTokenType type) {
	return type == KAR_TOKEN_IDENTIFIER;
}

bool kar_token_type_is_value(const KarTokenType type) {
	return type >= KAR_TOKEN_VAL_STRING && type <= KAR_TOKEN_VAL_MINUS_INFINITY;
}

bool kar_token_type_is_variable(const KarTokenType type) {
	return type >= KAR_TOKEN_VAR_BOOL && type <= KAR_TOKEN_VAR_STRING;
}

bool kar_token_type_is_sign(const KarTokenType type) {
	return type >= KAR_TOKEN_SIGN_OPEN_BRACES && type <= KAR_TOKEN_SIGN_BIT_LEFT;
}
