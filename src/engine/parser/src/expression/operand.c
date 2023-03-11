/* Copyright © 2021-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/string.h"
#include "core/alloc.h"
#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

// ----------------------------------------------------------------------------
// Общие функции.
// ----------------------------------------------------------------------------

static bool is_token_type_in_list(KarTokenType type, size_t num, const KarTokenType* list) {
	for (size_t i = 0; i < num; ++i) {
		if (type == list[i]) {
			return true;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
// Операторы с двумя операндами (А оп Б).
// ----------------------------------------------------------------------------

static bool make_two_operators(KarToken* token, size_t num, const KarTokenType* operands, KarProjectErrorList* errors) {
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (!is_token_type_in_list(child->type, num, operands)) {
			continue;
		}
		
		if (i == 0) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Нет первого операнда у операции.");
			return false;
		}
		if (i == kar_token_child_count(token) - 1) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Нет второго операнда у операции.");
			return false;
		}
		KarToken* first = kar_token_child_get(token, i - 1);
		KarToken* second = kar_token_child_get(token, i + 1);
		if (!kar_parser_is_expression(first->type)) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Первый оператор не корректен.");
			return false;
		}
		if (!kar_parser_is_expression(second->type)) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Второй оператор не корректен.");
			return false;
		}
		if (is_token_type_in_list(second->type, num, operands)) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Следующий операнд того же порядка что и текущий.");
			return false;
		}
		kar_token_child_tear(token, i + 1);
		kar_token_child_tear(token, i - 1);
		kar_token_child_add(child, first);
		kar_token_child_add(child, second);
		i--;
	}
	return true;
}

static bool foreach_two_operators(KarToken* token, size_t num, const KarTokenType* operands, KarProjectErrorList* errors) 
{
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		if (!foreach_two_operators(kar_token_child_get(token, i), num, operands, errors)) {
			return false;
		}
	}
	return make_two_operators(token, num, operands, errors);
}

// ----------------------------------------------------------------------------
// Операторы с операндом перед оператором (А оп).
// ----------------------------------------------------------------------------

static bool make_operator_before(KarToken* token, KarTokenType operator, KarProjectErrorList* errors) {
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (child->type != operator) {
			continue;
		}
		if (i == 0) {
			KarString* error_text = kar_string_create_format("Нет первого операнда у операции \"%s\".", child->str);
			kar_project_error_list_create_add(errors, &child->cursor, 1, error_text);
			KAR_FREE(error_text);
			return false;
		}
		
		KarToken* operator = kar_token_child_get(token, i - 1);
		if (!kar_parser_is_expression(operator->type)) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Оператор не корректен.");
			return false;
		}
		
		kar_token_child_tear(token, i - 1);
		kar_token_child_add(child, operator);
		i--;
	}
	return true;
}

static bool foreach_operator_before(KarToken* token, KarTokenType operator, KarProjectErrorList* errors) 
{
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		if (!foreach_operator_before(kar_token_child_get(token, i), operator, errors)) {
			return false;
		}
	}
	return make_operator_before(token, operator, errors);
}

// ----------------------------------------------------------------------------
// Операторы с операндом перед оператором (оп А).
// ----------------------------------------------------------------------------

static bool make_operator_after(KarToken* token, size_t op_num, const KarTokenType* operands, KarProjectErrorList* errors) {
	for (size_t num = kar_token_child_count(token); num > 0; --num) {
		size_t i = num - 1;
		KarToken* child = kar_token_child_get(token, i);
		if (!is_token_type_in_list(child->type, op_num, operands)) {
			continue;
		}
		if (i == kar_token_child_count(token) - 1) {
			KarString* error_text = kar_string_create_format("Нет операнда у операции \"%s\".", child->str);
			kar_project_error_list_create_add(errors, &child->cursor, 1, error_text);
			KAR_FREE(error_text);
			return false;
		}
		
		KarToken* operator = kar_token_child_get(token, i + 1);
		if (!kar_parser_is_expression(operator->type)) {
			kar_project_error_list_create_add(errors, &child->cursor, 1, "Оператор не корректен.");
			return false;
		}
		
		kar_token_child_tear(token, i + 1);
		kar_token_child_add(child, operator);
	}
	return true;
}

static bool foreach_operator_after(KarToken* token, size_t op_num, const KarTokenType* operands, KarProjectErrorList* errors) 
{
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		if (!foreach_operator_after(kar_token_child_get(token, i), op_num, operands, errors)) {
			return false;
		}
	}
	return make_operator_after(token, op_num, operands, errors);
}

// ----------------------------------------------------------------------------
// Операторы с операндом перед оператором (Поиск унарного плюса и минуса).
// ----------------------------------------------------------------------------

static void switch_to_single(KarToken* token) {
	if (token->type == KAR_TOKEN_SIGN_PLUS) {
		token->type = KAR_TOKEN_SIGN_SINGLE_PLUS;
	} else if (token->type == KAR_TOKEN_SIGN_MINUS) {
		token->type = KAR_TOKEN_SIGN_SINGLE_MINUS;
	}
}

static bool find_single_plus_minus(KarToken* token, KarProjectErrorList* errors) {
	if (errors) {
		
	}
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (child->type != KAR_TOKEN_SIGN_PLUS && child->type != KAR_TOKEN_SIGN_MINUS) {
			continue;
		}
		if (i == 0) {
			switch_to_single(child);
			continue;
		}
		KarToken* prev = kar_token_child_get(token, i - 1);
		if (
			kar_token_type_is_value(prev->type) ||
			prev->type == KAR_TOKEN_IDENTIFIER ||
			prev->type == KAR_TOKEN_SIGN_OPEN_BRACES ||
			prev->type == KAR_TOKEN_SIGN_UNCLEAN ||
			prev->type == KAR_TOKEN_SIGN_CLEAN
		) {
			continue;
		}
		switch_to_single(child);
	}
	return true;
}

static bool foreach_find_single(KarToken* token, KarProjectErrorList* errors) 
{
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		if (!foreach_find_single(kar_token_child_get(token, i), errors)) {
			return false;
		}
	}
	return find_single_plus_minus(token, errors);
}

// ----------------------------------------------------------------------------
// Основная сборная часть модуля.
// ----------------------------------------------------------------------------

static const KarTokenType OPERAND_LIST_CLEAN[] = {
	KAR_TOKEN_SIGN_CLEAN
};
static size_t OPERAND_LIST_CLEAN_SIZE = sizeof(OPERAND_LIST_CLEAN) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_SINGLE_NOT[] = {
	KAR_TOKEN_SIGN_SINGLE_PLUS,
	KAR_TOKEN_SIGN_SINGLE_MINUS,
	KAR_TOKEN_SIGN_NOT,
	KAR_TOKEN_SIGN_BIT_NOT
};
static size_t OPERAND_LIST_SINGLE_NOT_SIZE = sizeof(OPERAND_LIST_SINGLE_NOT) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_MUL_DIV[] = {
	KAR_TOKEN_SIGN_MUL,
	KAR_TOKEN_SIGN_DIV,
	KAR_TOKEN_SIGN_MOD,
	KAR_TOKEN_SIGN_DIV_CLEAN,
	KAR_TOKEN_SIGN_MOD_CLEAN
};
static size_t OPERAND_LIST_MUL_DIV_SIZE = sizeof(OPERAND_LIST_MUL_DIV) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_PLUS_MINUS[] = {
	KAR_TOKEN_SIGN_PLUS,
	KAR_TOKEN_SIGN_MINUS
};
static size_t OPERAND_LIST_PLUS_MINUS_SIZE = sizeof(OPERAND_LIST_PLUS_MINUS) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_RIGHT_LEFT[] = {
	KAR_TOKEN_SIGN_BIT_RIGHT,
	KAR_TOKEN_SIGN_BIT_LEFT
};
static size_t OPERAND_LIST_RIGHT_LEFT_SIZE = sizeof(OPERAND_LIST_RIGHT_LEFT) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_GREATER_LESS[] = {
	KAR_TOKEN_SIGN_GREATER,
	KAR_TOKEN_SIGN_GREATER_OR_EQUAL,
	KAR_TOKEN_SIGN_LESS,
	KAR_TOKEN_SIGN_LESS_OR_EQUAL
};
static size_t OPERAND_LIST_GREATER_LESS_SIZE = sizeof(OPERAND_LIST_GREATER_LESS) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_EQUAL_NOT_EQUAL[] = {
	KAR_TOKEN_SIGN_EQUAL,
	KAR_TOKEN_SIGN_NOT_EQUAL
};
static size_t OPERAND_LIST_EQUAL_NOT_EQUAL_SIZE = sizeof(OPERAND_LIST_EQUAL_NOT_EQUAL) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_BIT_AND[] = {
	KAR_TOKEN_SIGN_BIT_AND
};
static size_t OPERAND_LIST_BIT_AND_SIZE = sizeof(OPERAND_LIST_BIT_AND) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_BIT_XOR[] = {
	KAR_TOKEN_SIGN_BIT_XOR
};
static size_t OPERAND_LIST_BIT_XOR_SIZE = sizeof(OPERAND_LIST_BIT_XOR) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_BIT_OR[] = {
	KAR_TOKEN_SIGN_BIT_OR
};
static size_t OPERAND_LIST_BIT_OR_SIZE = sizeof(OPERAND_LIST_BIT_OR) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_AND[] = {
	KAR_TOKEN_SIGN_AND
};
static size_t OPERAND_LIST_AND_SIZE = sizeof(OPERAND_LIST_AND) / sizeof(KarTokenType);

static const KarTokenType OPERAND_LIST_OR[] = {
	KAR_TOKEN_SIGN_OR
};
static size_t OPERAND_LIST_OR_SIZE = sizeof(OPERAND_LIST_OR) / sizeof(KarTokenType);

bool kar_parser_make_operands(KarToken* token, KarProjectErrorList* errors)
{
	bool b = true;
	
	b = b && foreach_operator_before(token, KAR_TOKEN_SIGN_UNCLEAN, errors);
	b = b && foreach_two_operators(token, OPERAND_LIST_CLEAN_SIZE, OPERAND_LIST_CLEAN, errors);
	
	b = b && foreach_find_single(token, errors);
	b = b && foreach_operator_after(token, OPERAND_LIST_SINGLE_NOT_SIZE, OPERAND_LIST_SINGLE_NOT, errors);

	b = b && foreach_two_operators(token, OPERAND_LIST_MUL_DIV_SIZE, OPERAND_LIST_MUL_DIV, errors);
	b = b && foreach_two_operators(token, OPERAND_LIST_PLUS_MINUS_SIZE, OPERAND_LIST_PLUS_MINUS, errors);
	
	b = b && foreach_two_operators(token, OPERAND_LIST_RIGHT_LEFT_SIZE, OPERAND_LIST_RIGHT_LEFT, errors);
	b = b && foreach_two_operators(token, OPERAND_LIST_GREATER_LESS_SIZE, OPERAND_LIST_GREATER_LESS, errors);
	b = b && foreach_two_operators(token, OPERAND_LIST_EQUAL_NOT_EQUAL_SIZE, OPERAND_LIST_EQUAL_NOT_EQUAL, errors);

	b = b && foreach_two_operators(token, OPERAND_LIST_BIT_AND_SIZE, OPERAND_LIST_BIT_AND, errors);
	b = b && foreach_two_operators(token, OPERAND_LIST_BIT_XOR_SIZE, OPERAND_LIST_BIT_XOR, errors);
	b = b && foreach_two_operators(token, OPERAND_LIST_BIT_OR_SIZE, OPERAND_LIST_BIT_OR, errors);
	b = b && foreach_two_operators(token, OPERAND_LIST_AND_SIZE, OPERAND_LIST_AND, errors);
	b = b && foreach_two_operators(token, OPERAND_LIST_OR_SIZE, OPERAND_LIST_OR, errors);
	
	return b;
}