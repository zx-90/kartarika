/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_TOKEN_TYPE_H
#define KAR_TOKEN_TYPE_H

#include <stdbool.h>

typedef enum {
	KAR_TOKEN_UNKNOWN,
	
	// Первоначальные
	KAR_TOKEN_MODULE,
	KAR_TOKEN_COMMENT,
	KAR_TOKEN_INDENT,
	KAR_TOKEN_SPACE,
	KAR_TOKEN_IDENTIFIER,
	KAR_TOKEN_SIGN,
	KAR_TOKEN_VAL_STRING,
	
	// Значения переменых.
	KAR_TOKEN_VAL_NULL,
	KAR_TOKEN_VAL_TRUE,
	KAR_TOKEN_VAL_FALSE,
	KAR_TOKEN_VAL_INTEGER,
	KAR_TOKEN_VAL_HEXADECIMAL,
	KAR_TOKEN_VAL_FLOAT,
	KAR_TOKEN_VAL_NAN,
	KAR_TOKEN_VAL_INFINITY,
	KAR_TOKEN_VAL_MINUS_INFINITY,
	
	// Поля
	KAR_TOKEN_FIELD_CONST,
	KAR_TOKEN_FIELD_VAR,
	
	// Модификаторы поля
	// TODO: Переименовать в MODIFIERS (множественное число)
	KAR_TOKEN_VAR_MODIFIER,
	KAR_TOKEN_VAR_MODIFIER_DYNAMIC,
	KAR_TOKEN_VAR_MODIFIER_PRIVATE,
	KAR_TOKEN_VAR_MODIFIER_PROTECTED,
	KAR_TOKEN_VAR_MODIFIER_PUBLIC,

	// Простые переменные
	KAR_TOKEN_VAR_BOOL,
	
	KAR_TOKEN_VAR_INTEGER8,
	KAR_TOKEN_VAR_INTEGER16,
	KAR_TOKEN_VAR_INTEGER32,
	KAR_TOKEN_VAR_INTEGER64,
	
	KAR_TOKEN_VAR_UNSIGNED8,
	KAR_TOKEN_VAR_UNSIGNED16,
	KAR_TOKEN_VAR_UNSIGNED32,
	KAR_TOKEN_VAR_UNSIGNED64,
	
	KAR_TOKEN_VAR_FLOAT32,
	KAR_TOKEN_VAR_FLOAT64,
	KAR_TOKEN_VAR_FLOAT80,
	
	KAR_TOKEN_VAR_STRING,
	
	// Знаки
	KAR_TOKEN_SIGN_OPEN_BRACES,
	KAR_TOKEN_SIGN_CLOSE_BRACES,
	
	KAR_TOKEN_SIGN_GET_FIELD,
	KAR_TOKEN_SIGN_CALL_METHOD,
	KAR_TOKEN_SIGN_ARGUMENT,
	KAR_TOKEN_SIGN_UNCLEAN,
	KAR_TOKEN_SIGN_CLEAN,
	
	KAR_TOKEN_SIGN_COMMA,
	KAR_TOKEN_SIGN_COLON,

	KAR_TOKEN_SIGN_ASSIGN,

	KAR_TOKEN_SIGN_SINGLE_PLUS,
	KAR_TOKEN_SIGN_SINGLE_MINUS,
	KAR_TOKEN_SIGN_PLUS,
	KAR_TOKEN_SIGN_MINUS,
	KAR_TOKEN_SIGN_MUL,
	KAR_TOKEN_SIGN_DIV,
	KAR_TOKEN_SIGN_MOD,
	KAR_TOKEN_SIGN_MUL_CLEAN, // TODO: Похоже лишнее
	KAR_TOKEN_SIGN_DIV_CLEAN,
	KAR_TOKEN_SIGN_MOD_CLEAN,
	
	KAR_TOKEN_SIGN_AND,
	KAR_TOKEN_SIGN_OR,
	KAR_TOKEN_SIGN_NOT,
	KAR_TOKEN_SIGN_EQUAL,
	KAR_TOKEN_SIGN_NOT_EQUAL,
	KAR_TOKEN_SIGN_GREATER,
	KAR_TOKEN_SIGN_GREATER_OR_EQUAL,
	KAR_TOKEN_SIGN_LESS,
	KAR_TOKEN_SIGN_LESS_OR_EQUAL,
	
	KAR_TOKEN_SIGN_BIT_AND,
	KAR_TOKEN_SIGN_BIT_OR,
	KAR_TOKEN_SIGN_BIT_NOT,
	KAR_TOKEN_SIGN_BIT_XOR,
	KAR_TOKEN_SIGN_BIT_RIGHT,
	KAR_TOKEN_SIGN_BIT_LEFT,
	
	// Команды алгоритма.
	KAR_TOKEN_COMMAND_EXPRESSION,
	KAR_TOKEN_COMMAND_RETURN,
	KAR_TOKEN_COMMAND_DECLARATION,
	KAR_TOKEN_COMMAND_ASSIGN,
	
	KAR_TOKEN_COMMAND_BLOCK,
	KAR_TOKEN_COMMAND_PASS,
	
	KAR_TOKEN_COMMAND_CLEAN,
	KAR_TOKEN_COMMAND_AS,
	
	KAR_TOKEN_COMMAND_IF,
	KAR_TOKEN_COMMAND_ELSE,
	
	KAR_TOKEN_COMMAND_WHILE,
	KAR_TOKEN_COMMAND_CONTINUE,
	KAR_TOKEN_COMMAND_BREAK,
	
	KAR_TOKEN_BLOCK_BODY,
	
	// Функция.
	KAR_TOKEN_METHOD,
	
	// TODO: Переименовать в MODIFIERS (множественное число)
	KAR_TOKEN_METHOD_MODIFIER,
	KAR_TOKEN_MODIFIER_STAT,
	KAR_TOKEN_METHOD_MODIFIER_DYNAMIC,
	KAR_TOKEN_METHOD_MODIFIER_PRIVATE,
	KAR_TOKEN_METHOD_MODIFIER_PROTECTED,
	KAR_TOKEN_METHOD_MODIFIER_PUBLIC,
	KAR_TOKEN_METHOD_MODIFIER_FINALIZED,
	KAR_TOKEN_METHOD_MODIFIER_INHERITED,
	KAR_TOKEN_METHOD_MODIFIER_OVERLOAD,

	
	KAR_TOKEN_METHOD_PARAMETERS,
	KAR_TOKEN_METHOD_PARAMETER_CONST,
	KAR_TOKEN_METHOD_PARAMETER_VAR,
	
	KAR_TOKEN_METHOD_RETURN_TYPE,
} KarTokenType;

const char* kar_token_type_get_name(const KarTokenType type);

bool kar_token_type_is_identifier(const KarTokenType type);
bool kar_token_type_is_value(const KarTokenType type);
bool kar_token_type_is_variable(const KarTokenType type);
bool kar_token_type_is_sign(const KarTokenType type);

#endif // KAR_TOKEN_TYPE_H
