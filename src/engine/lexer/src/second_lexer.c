/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/second_lexer.h"

#include <string.h>

#include "core/string.h"
#include "core/alloc.h"
#include "lexer/keyword.h"
#include "lexer/check_alphabet.h"

static void retype_if_check(KarToken* token, KarTokenType checkType, const char* str, KarTokenType newType) {
	if (token->type == checkType && !strcmp(token->str, str)) {
		token->type = newType;
	}
}

static bool is_cypher(const char c) {
	return (c >= 0x30 && c <= 0x39);
}

static bool is_exponent(const char* str) {
	return (memcmp(str, "С", 2) == 0
		|| memcmp(str, "с", 2) == 0);
}

enum KarExpPosition {
	KAR_EXP_POSITION_END,
	KAR_EXP_POSITION_MIDDLE,
	KAR_EXP_POSITION_ERROR
};

static enum KarExpPosition get_exp_status(const char* str) {
	if (is_exponent(str)) {
		return KAR_EXP_POSITION_ERROR;
	}
	enum KarExpPosition result = KAR_EXP_POSITION_ERROR;
	while (*str != 0) {
		if (is_cypher(*str)) {
			if (result == KAR_EXP_POSITION_END) {
				result = KAR_EXP_POSITION_MIDDLE;
			}
		}
		else if (is_exponent(str)) {
			if (result == KAR_EXP_POSITION_ERROR) {
				result = KAR_EXP_POSITION_END;
			}
			else {
				return KAR_EXP_POSITION_ERROR;
			}
			str++;
		}
		else {
			return KAR_EXP_POSITION_ERROR;
		}
		str++;
	}
	return result;
}

static bool check_for_number(KarToken* token, KarModule* module) {
	// TODO: Плюс бесконеность, минус бесконечность, +0, -0, НеЧисло.
	if (token->type != KAR_TOKEN_IDENTIFIER) {
		return true;
	}
	if (!is_cypher(token->str[0])) {
		return true;
	}

	if (get_exp_status(token->str) != KAR_EXP_POSITION_ERROR) {
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
	// TODO: Не обязательно передавать KarModule. Достаточно KarModuleError.
	if (token->type == KAR_TOKEN_IDENTIFIER && !kar_check_identifiers_alphabet(token->str)) {
		kar_module_error_create_add(&module->errors, &token->cursor, 1, "Токен составлен из разных алфавитов.");
		return false;
	}
	// Значения переменных.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_NULL, KAR_TOKEN_VAL_NULL);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_TRUE, KAR_TOKEN_VAL_TRUE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_FALSE, KAR_TOKEN_VAL_FALSE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_NAN, KAR_TOKEN_VAL_NAN);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_INFINITY, KAR_TOKEN_VAL_INFINITY);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_PLUS_INFINITY, KAR_TOKEN_VAL_INFINITY);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_VAL_MINUS_INFINITY, KAR_TOKEN_VAL_MINUS_INFINITY);
	
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
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_UNCLEAN, KAR_TOKEN_SIGN_UNCLEAN);
	retype_if_check(token, KAR_TOKEN_SIGN, KAR_KEYWORD_SIGN_CLEAN, KAR_TOKEN_SIGN_CLEAN);
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
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_GREATER, KAR_TOKEN_SIGN_GREATER);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_GREATER_OR_EQUAL, KAR_TOKEN_SIGN_GREATER_OR_EQUAL);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_LESS, KAR_TOKEN_SIGN_LESS);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_LESS_OR_EQUAL, KAR_TOKEN_SIGN_LESS_OR_EQUAL);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_AND, KAR_TOKEN_SIGN_BIT_AND);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_OR, KAR_TOKEN_SIGN_BIT_OR);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_NOT, KAR_TOKEN_SIGN_BIT_NOT);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_XOR, KAR_TOKEN_SIGN_BIT_XOR);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_RIGHT, KAR_TOKEN_SIGN_BIT_RIGHT);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, KAR_KEYWORD_SIGN_BIT_LEFT, KAR_TOKEN_SIGN_BIT_LEFT);
	
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

static KarTokenType CONCAT_LIST[][3] = {
	{ KAR_TOKEN_SIGN_MUL, KAR_TOKEN_SIGN_CLEAN, KAR_TOKEN_SIGN_MUL_CLEAN },
	{ KAR_TOKEN_SIGN_DIV, KAR_TOKEN_SIGN_CLEAN, KAR_TOKEN_SIGN_DIV_CLEAN },
	{ KAR_TOKEN_SIGN_MOD, KAR_TOKEN_SIGN_CLEAN, KAR_TOKEN_SIGN_MOD_CLEAN },
	{ KAR_TOKEN_SIGN_ASSIGN, KAR_TOKEN_SIGN_ASSIGN, KAR_TOKEN_SIGN_EQUAL },
	{ KAR_TOKEN_SIGN_CLEAN, KAR_TOKEN_SIGN_ASSIGN, KAR_TOKEN_SIGN_NOT_EQUAL }
};
static size_t CONCAT_LIST_SIZE = sizeof(CONCAT_LIST) / sizeof(KarTokenType[3]);

static bool concat_signs(KarToken* token) {
	if (token->children.count == 0) {
		return true;
	}
	for (size_t i = 0; i < token->children.count - 1; ++i) {
		KarToken* cur = kar_token_child(token, i);
		KarToken* next = kar_token_child(token, i + 1);
		KarTokenType curType = cur->type;
		KarTokenType nextType = next->type;
		for (size_t j = 0; j < CONCAT_LIST_SIZE; ++j) {
			if (curType == CONCAT_LIST[j][0] && nextType == CONCAT_LIST[j][1]) {
				cur->type = CONCAT_LIST[j][2];
				kar_token_join_children(token, i, 2);
			}
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// floats
// ----------------------------------------------------------------------------

static bool find_floats_5(KarToken* token, KarArray* errors) {
	for (size_t i = 0; i < token->children.count - 4; ++i) {
		if (token->children.count < 5) {
			return true;
		}
		KarToken* first = kar_token_child(token, i);
		KarToken* second = kar_token_child(token, i + 1);
		KarToken* third = kar_token_child(token, i + 2);
		KarToken* fourth = kar_token_child(token, i + 3);
		KarToken* fifth = kar_token_child(token, i + 4);

		if (first->type != KAR_TOKEN_VAL_INTEGER) {
			continue;
		}
		if (second->type != KAR_TOKEN_SIGN_GET_FIELD) {
			continue;
		}
		if (third->type != KAR_TOKEN_IDENTIFIER && get_exp_status(third->str) == KAR_EXP_POSITION_END) {
			continue;
		}
		if (fourth->type != KAR_TOKEN_SIGN_MINUS && fourth->type != KAR_TOKEN_SIGN_PLUS) {
			continue;
		}
		if (fifth->type != KAR_TOKEN_VAL_INTEGER) {
			continue;
		}

		first->type = KAR_TOKEN_VAL_FLOAT;
		kar_token_join_children(token, i, 5);
	}
	return true;
}

static bool find_floats_3(KarToken* token, KarArray* errors) {
	for (size_t i = 0; i < token->children.count - 2; ++i) {
		if (token->children.count < 3) {
			return true;
		}
		KarToken* integer = kar_token_child(token, i);
		KarToken* dot = kar_token_child(token, i + 1);
		KarToken* fraction = kar_token_child(token, i + 2);

		if (integer->type == KAR_TOKEN_VAL_INTEGER && dot->type == KAR_TOKEN_SIGN_GET_FIELD && fraction->type == KAR_TOKEN_VAL_INTEGER) {
			integer->type = KAR_TOKEN_VAL_FLOAT;
			kar_token_join_children(token, i, 3);
		}
		if (integer->type == KAR_TOKEN_VAL_INTEGER && dot->type == KAR_TOKEN_SIGN_GET_FIELD && fraction->type == KAR_TOKEN_IDENTIFIER && get_exp_status(fraction->str) == KAR_EXP_POSITION_MIDDLE) {
			integer->type = KAR_TOKEN_VAL_FLOAT;
			kar_token_join_children(token, i, 3);
		}
		if (integer->type == KAR_TOKEN_IDENTIFIER && 
			get_exp_status(integer->str) == KAR_EXP_POSITION_END &&
			(dot->type == KAR_TOKEN_SIGN_MINUS || dot->type == KAR_TOKEN_SIGN_PLUS) &&
			fraction->type == KAR_TOKEN_VAL_INTEGER) {

			integer->type = KAR_TOKEN_VAL_FLOAT;
			kar_token_join_children(token, i, 3);
		}
	}
	return true;
}

static bool find_floats_2(KarToken* token, KarArray* errors) {
	for (size_t i = 0; i < token->children.count - 1; ++i) {
		if (token->children.count < 2) {
			return true;
		}
		KarToken* first = kar_token_child(token, i);
		KarToken* second = kar_token_child(token, i + 1);

		if (first->type == KAR_TOKEN_SIGN_GET_FIELD && second->type == KAR_TOKEN_VAL_INTEGER) {
			kar_module_error_create_add(errors, &first->cursor, 1, "Некорректная запись числа с плавающей точкой.");
			return false;
		}
		if (first->type == KAR_TOKEN_VAL_INTEGER && second->type == KAR_TOKEN_SIGN_GET_FIELD) {
			kar_module_error_create_add(errors, &first->cursor, 1, "Некорректная запись числа с плавающей точкой.");
			return false;
		}
	}
	return true;
}

static bool find_floats_1(KarToken* token, KarArray* errors) {
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* first = kar_token_child(token, i);

		if (first->type == KAR_TOKEN_IDENTIFIER && get_exp_status(first->str) == KAR_EXP_POSITION_END) {
			kar_module_error_create_add(errors, &first->cursor, 1, "Некорректная запись числа с плавающей точкой.");
			return false;
		}
		if (first->type == KAR_TOKEN_IDENTIFIER && get_exp_status(first->str) == KAR_EXP_POSITION_MIDDLE) {
			first->type = KAR_TOKEN_VAL_FLOAT;
			return true;
		}
	}
	return true;
}

static bool find_floats(KarToken* token, KarArray* errors) {	
	
	if (!find_floats_5(token, errors)) {
		return false;
	}
	if (!find_floats_3(token, errors)) {
		return false;
	}
	if (!find_floats_2(token, errors)) {
		return false;
	}
	if (!find_floats_1(token, errors)) {
		return false;
	}
	return true;
}

static bool check_identifires(KarToken* token, KarArray* errors) {
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		if (child->type == KAR_TOKEN_IDENTIFIER && is_cypher(*child->str)) {
			kar_module_error_create_add(errors, &child->cursor, 1, "Идентификатор начинается с цифры.");
			return false;
		}
	}
	return true;
}

bool kar_second_lexer_run(KarModule* module) {
	return
		foreach_retype(module->token, module) &&
		concat_signs(module->token) &&
		find_floats(module->token, &module->errors) &&
		check_identifires(module->token, &module->errors);
}
