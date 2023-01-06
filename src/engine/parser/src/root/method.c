/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/alloc.h"
#include "core/string.h"
#include "structure/token.h"
#include "structure/module_error.h"
#include "parser/base.h"

static bool parse_modifiers(KarToken* token, size_t methodPos, KarArray* errors) {
	KarToken* modifiers = kar_token_create();
	modifiers->type = KAR_TOKEN_METHOD_MODIFIER;
	modifiers->cursor = kar_token_child(token, 0)->cursor;
	kar_token_child_insert(token, modifiers, 0);
	kar_token_child_move_to_end(token, modifiers, 1, methodPos);
	
	bool static_modifier = false;
	bool inherit_modifier = false;
	bool finalize_modifier = false;
	bool overload_modifier = false;
	for (size_t i = 0; i < modifiers->children.count; i++){
		KarToken* token = kar_token_child(modifiers, i);
		if (
			token->type != KAR_TOKEN_MODIFIER_STAT &&
			token->type != KAR_TOKEN_METHOD_MODIFIER_DYNAMIC &&
			token->type != KAR_TOKEN_METHOD_MODIFIER_PRIVATE &&
			token->type != KAR_TOKEN_METHOD_MODIFIER_PROTECTED &&
			token->type != KAR_TOKEN_METHOD_MODIFIER_PUBLIC &&
			token->type != KAR_TOKEN_METHOD_MODIFIER_FINALIZED &&
			token->type != KAR_TOKEN_METHOD_MODIFIER_INHERITED &&
			token->type != KAR_TOKEN_METHOD_MODIFIER_OVERLOAD
		) {
			kar_module_error_create_add(errors, &token->cursor, 1, "Некорректный модификатор при объявлении метода.");
			return false;
		}
		if (
			token->type == KAR_TOKEN_MODIFIER_STAT ||
			token->type == KAR_TOKEN_METHOD_MODIFIER_DYNAMIC
		) {
			if (static_modifier) {
				kar_module_error_create_add(errors, &token->cursor, 1, "Объявлено более одного модификатора статичности при объявлении метода.");
				return false;
			} else {
				static_modifier = true;
			}
		}
		if (
			token->type == KAR_TOKEN_METHOD_MODIFIER_PRIVATE ||
			token->type == KAR_TOKEN_METHOD_MODIFIER_PROTECTED ||
			token->type == KAR_TOKEN_METHOD_MODIFIER_PUBLIC
		) {
			if (inherit_modifier) {
				kar_module_error_create_add(errors, &token->cursor, 1, "Объявлено более одного модификатора области видимости при объявлении метода.");
				return false;
			} else {
				inherit_modifier = true;
			}
		}
		if (
			token->type == KAR_TOKEN_METHOD_MODIFIER_FINALIZED ||
			token->type == KAR_TOKEN_METHOD_MODIFIER_INHERITED
		) {
			if (finalize_modifier) {
				kar_module_error_create_add(errors, &token->cursor, 1, "Объявлено более одного модификатора финализации при объявлении метода.");
				return false;
			} else {
				finalize_modifier = true;
			}
		}
		if (
			token->type == KAR_TOKEN_METHOD_MODIFIER_OVERLOAD
		) {
			if (overload_modifier) {
				kar_module_error_create_add(errors, &token->cursor, 1, "Объявлено более одного модификатора перегрузки при объявлении метода.");
				return false;
			} else {
				overload_modifier = true;
			}
		}
	}
	return true;
}

static void parse_keyword(KarToken* token) {
	const size_t CHILD_INDEX = 1;
	kar_token_child_erase(token, CHILD_INDEX);
}

static bool parse_method_name(KarToken* token, KarArray* errors) {
	const size_t CHILD_INDEX = 1;
	if (CHILD_INDEX == token->children.count) {
		kar_module_error_create_add(errors, &kar_token_child(token, token->children.count - 1)->cursor, 1, "Неожиданный конец строки. Ожидалось имя метода.");
		return false;
	}
	
	KarToken* methodParams = kar_token_child(token, CHILD_INDEX);
	if (methodParams->type != KAR_TOKEN_SIGN_CALL_METHOD) {
		kar_module_error_create_add(errors, &methodParams->cursor, 1, "Некорректное имя метода. Ожидалось, что будет идентификатор.");
		return false;
	}
	if (methodParams->children.count < 1) {
		kar_module_error_create_add(errors, &methodParams->cursor, 1, "Не возможно найти имя метода.");
		return false;
	}
	
	KarToken* methodName = kar_token_child(methodParams, 0);
	if (methodName->type != KAR_TOKEN_IDENTIFIER) {
		kar_module_error_create_add(errors, &methodName->cursor, 1, "Не корректное имя метода.");
		return false;
	}
	
	token->type = KAR_TOKEN_METHOD;
	token->str = kar_string_create_copy(methodName->str);
	kar_token_child_erase(methodParams, 0);
	
	/*if (methodParams->children.count > 0) {
		methodParams->cursor = kar_token_child(methodParams, 0)->cursor;
	} else {*/
		// TODO: Сделать функцию в cursor.c, которая бы по строке вычисляла положение курсора с учетом символов новой строки.
		methodParams->cursor.column +=  (int)kar_string_length(token->str);
	//}
	
	return true;
}

static bool parse_method_parameters(KarToken* token, KarArray* errors) {
	const size_t CHILD_INDEX = 1;
	KarToken* parameters = kar_token_child_tear(token, CHILD_INDEX);
	parameters->type = KAR_TOKEN_METHOD_PARAMETERS;
	KAR_FREE(parameters->str);
	parameters->str = NULL;
	kar_token_child_insert(token, parameters, 1);
	
	for (size_t i = 0; i < parameters->children.count; i++) {
		KarToken* parameter = kar_token_child(parameters, i);
		if (parameter->children.count == 1) {
			KarToken* mul = kar_token_child(parameter, 0);
			if (mul->type != KAR_TOKEN_SIGN_MUL) {
				kar_module_error_create_add(errors, &parameter->cursor, 1, "Некорректный параметр метода.");
				return false;
			}
			if (mul->children.count != 2) {
				kar_module_error_create_add(errors, &parameter->cursor, 1, "Некорректный параметр метода.");
				return false;
			}
			parameter->type = KAR_TOKEN_METHOD_PARAMETER_VAR;
			kar_token_child_tear(parameter, 0);
			KarToken* type = kar_token_child_tear(mul, 0);
			KarToken* name = kar_token_child_tear(mul, 0);
			kar_token_child_add(parameter, type);
			kar_token_child_add(parameter, name);
			kar_token_free(mul);
			
		} else if (parameter->children.count == 2) {
			parameter->type = KAR_TOKEN_METHOD_PARAMETER_CONST;
		} else {
			kar_module_error_create_add(errors, &parameter->cursor, 1, "Некорректный параметр метода.");
			return false;
		}
		
		if (parameter->children.count != 2) {
			kar_module_error_create_add(errors, &parameter->cursor, 1, "Некорректный параметр метода.");
			return false;
		}
		KarToken* type = kar_token_child(parameter, 0);
		KarToken* name = kar_token_child(parameter, 1);
		if (!kar_token_is_type(type->type)) {
			kar_module_error_create_add(errors, &type->cursor, 1, "Некорректный тип параметра метода.");
			return false;
		}
		if (!kar_token_is_name(name->type)) {
			kar_module_error_create_add(errors, &name->cursor, 1, "Некорректное имя параметра метода.");
			return false;
		}
	}
	return true;
}

static bool parse_return_type(KarToken* token, KarArray* errors) {
	const size_t CHILD_INDEX = 2;
	if (CHILD_INDEX == token->children.count) {
		return false;
	}
	KarToken* returnType = kar_token_create();
	returnType->type = KAR_TOKEN_METHOD_RETURN_TYPE;
	returnType->cursor = kar_token_child(token, CHILD_INDEX)->cursor;
	// TODO: Надо проверять корректность типа возвращаемого значения. Это либо идентификатор, либо стандартный тип.
	if (kar_token_child(token, CHILD_INDEX)->type != KAR_TOKEN_SIGN_COLON) {
		KarToken* returnTypeToken = kar_token_child_tear(token, CHILD_INDEX);
		kar_token_child_add(returnType, returnTypeToken);
		if (!kar_token_is_type(returnTypeToken->type)) {
			kar_module_error_create_add(errors, &returnTypeToken->cursor, 1, "Некорректный тип возвращаемого значения функции.");
			return false;
		}
	}
	kar_token_child_insert(token, returnType, CHILD_INDEX);
	return true;
}

static bool parse_colon(KarToken* token, KarArray* errors) {
	const size_t CHILD_INDEX = 3;
	if (CHILD_INDEX == token->children.count) {
		// TODO: Надо указать курсор на конец токена.
		kar_module_error_create_add(errors, &kar_token_child(token, CHILD_INDEX - 1)->cursor, 1, "Пропущено двоеточие в конце строки.");
		return false;
	}
	if (kar_token_child(token, CHILD_INDEX)->type != KAR_TOKEN_SIGN_COLON) {
		kar_module_error_create_add(errors, &kar_token_child(token, CHILD_INDEX)->cursor, 1, "Ожидался символ \":\".");
		return false;
	}
	if ((CHILD_INDEX + 1) == token->children.count) {
		kar_module_error_create_add(errors, &kar_token_child(token, CHILD_INDEX)->cursor, 1, "Неожиданное выражение в конце строки.");
		return false;
	}
	kar_token_child_erase(token, CHILD_INDEX);
	return true;
}

static bool parse_algorithm(KarToken* token, KarArray* errors) {
	const size_t CHILD_INDEX = 3;
	if (token->children.count == CHILD_INDEX) {
		// TODO: Надо указать курсор на конец токена.
		kar_module_error_create_add(errors, &kar_token_child(token, CHILD_INDEX - 1)->cursor, 1, "Отсутствует алгоритм метода.");
		return false;
	}
	if (token->children.count > CHILD_INDEX + 1) {
		kar_module_error_create_add(errors, &kar_token_child(token, CHILD_INDEX + 1)->cursor, 1, "Слишком много алгоритмов или других элементов в методе.");
		return false;
	}
	
	// TODO: Возможно эти 2 блока если на самом деле являются частью проверки алгоритма. Надо перенести туда.
	if (kar_token_child(token, CHILD_INDEX)->type != KAR_TOKEN_BLOCK_BODY) {
		kar_module_error_create_add(errors, &kar_token_child(token, CHILD_INDEX)->cursor, 1, "Здесь ожидалось найти тело метода.");
		return false;
	}
	if (kar_token_child(token, CHILD_INDEX)->children.count == 0) {
		kar_module_error_create_add(errors, &kar_token_child(token, CHILD_INDEX)->cursor, 1, "В теле метода должна быть хотя бы одна команда.");
		return false;
	}
	
	return kar_parser_parse_algorithm(kar_token_child(token, CHILD_INDEX), errors);
}

// ----------------------------------------------------------------------------

KarParserStatus kar_parser_make_method(KarToken* token, KarArray* errors)
{
	size_t methodPos = kar_token_child_find(token, KAR_TOKEN_METHOD);
	if (methodPos == token->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	if (!parse_modifiers(token, methodPos, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	methodPos = 1;
	
	parse_keyword(token);
	
	if (!parse_method_name(token, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (!parse_method_parameters(token, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (!parse_return_type(token, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (!parse_colon(token, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (!parse_algorithm(token, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	return KAR_PARSER_STATUS_PARSED;
}
