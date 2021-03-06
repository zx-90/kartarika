/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_TOKEN_TYPE_H
#define KAR_TOKEN_TYPE_H

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
	KAR_TOKEN_VAL_FLOAT,
	
	// Поля
	KAR_TOKEN_FIELD_CONST,
	KAR_TOKEN_FIELD_VAR,
	
	// Модификаторы поля
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
	
	// Составные переменные.
	KAR_TOKEN_ENUMERATION,
	KAR_TOKEN_STRUCTURE,
	KAR_TOKEN_UNION,
	
	// Знаки
	KAR_TOKEN_SIGN_NULLABLE,
	KAR_TOKEN_SIGN_OPEN_BRACES,
	KAR_TOKEN_SIGN_CLOSE_BRACES,
	KAR_TOKEN_SIGN_GET_FIELD,
	KAR_TOKEN_SIGN_COMMA,
	KAR_TOKEN_SIGN_COLON,

	KAR_TOKEN_SIGN_ASSIGN,

	KAR_TOKEN_SIGN_PLUS,
	KAR_TOKEN_SIGN_MINUS,
	KAR_TOKEN_SIGN_MUL,
	KAR_TOKEN_SIGN_DIV,
	KAR_TOKEN_SIGN_MOD,
	
	KAR_TOKEN_SIGN_AND,
	KAR_TOKEN_SIGN_OR,
	KAR_TOKEN_SIGN_NOT,
	KAR_TOKEN_SIGN_EQUAL,
	KAR_TOKEN_SIGN_GREATER,
	KAR_TOKEN_SIGN_LESS,
	
	KAR_TOKEN_SIGN_BIT_AND,
	KAR_TOKEN_SIGN_BIT_OR,
	KAR_TOKEN_SIGN_BIT_NOT,
	KAR_TOKEN_SIGN_BIT_XOR,
	
	// Управление последовательностью выполенения.
	KAR_TOKEN_COMMAND_BLOCK,
	KAR_TOKEN_COMMAND_PASS,
	
	KAR_TOKEN_COMMAND_IF,
	KAR_TOKEN_COMMANT_THEN,
	KAR_TOKEN_COMMAND_ELSE,
	
	KAR_TOKEN_COMMAND_FOR,
	KAR_TOKEN_COMMAND_FROM,
	KAR_TOKEN_COMMAND_TO,
	KAR_TOKEN_COMMAND_STEP,
	
	KAR_TOKEN_COMMAND_FOREACH,
	KAR_TOKEN_COMMAND_GET,
	
	KAR_TOKEN_COMMAND_WHILE,

	KAR_TOKEN_COMMAND_CONTINUE,
	KAR_TOKEN_COMMAND_BREAK,
	
	KAR_TOKEN_COMMAND_OPEN, // рыскрыть вопросительный элемент.
	
	KAR_TOKEN_BLOCK_BODY,
	
	// Функция.
	KAR_TOKEN_METHOD,
	
	KAR_TOKEN_METHOD_MODIFIER,
	//KAR_TOKEN_METHOD_PRIVATE,
	//KAR_TOKEN_METHOD_PROTECTED,
	//KAR_TOKEN_METHOD_PUBLIC,
	//KAR_TOKEN_METHOD_LOCAL,
	KAR_TOKEN_MODIFIER_STAT,
	
	KAR_TOKEN_METHOD_PARAMETERS,
	//KAR_TOKEN_METHOD_ARGUMENT_CONST,
	//KAR_TOKEN_METHOD_ARGUMENT_VAR,
	//KAR_TOKEN_METHOD_ARGUMENT_POINTER,
	
	KAR_TOKEN_METHOD_RETURN_TYPE,
	KAR_TOKEN_METHOD_RETURN,
	
	// Класс
	
	// Шаблон класса
	
	// Тест
	KAR_TOKEN_TEST,
	KAR_TOKEN_CHECK,
} KarTokenType;

const char* kar_token_type_get_name(const KarTokenType type);

#endif // KAR_TOKEN_TYPE_H
