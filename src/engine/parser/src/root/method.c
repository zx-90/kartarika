/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/alloc.h"
#include "core/string.h"
#include "core/unicode.h"
#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

static bool parse_modifiers(KarToken* token, size_t methodPos, KarString* moduleName, KarProjectErrorList* errors) {
	KarToken* modifiers = kar_token_create();
	modifiers->type = KAR_TOKEN_METHOD_MODIFIER;
	modifiers->cursor = kar_token_child_get(token, 0)->cursor;
	kar_token_child_insert(token, modifiers, 0);
	kar_token_child_move_to_end(token, modifiers, 1, methodPos);
	
	bool static_modifier = false;
	bool inherit_modifier = false;
	bool finalize_modifier = false;
	bool overload_modifier = false;
	for (size_t i = 0; i < kar_token_child_count(modifiers); i++){
		KarToken* token = kar_token_child_get(modifiers, i);
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
            kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Некорректный модификатор при объявлении метода.");
			return false;
		}
		if (
			token->type == KAR_TOKEN_MODIFIER_STAT ||
			token->type == KAR_TOKEN_METHOD_MODIFIER_DYNAMIC
		) {
			if (static_modifier) {
                kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Объявлено более одного модификатора статичности при объявлении метода.");
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
                kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Объявлено более одного модификатора области видимости при объявлении метода.");
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
                kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Объявлено более одного модификатора финализации при объявлении метода.");
				return false;
			} else {
				finalize_modifier = true;
			}
		}
		if (
			token->type == KAR_TOKEN_METHOD_MODIFIER_OVERLOAD
		) {
			if (overload_modifier) {
                kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Объявлено более одного модификатора перегрузки при объявлении метода.");
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

static bool parse_method_name(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	const size_t CHILD_INDEX = 1;
	if (CHILD_INDEX == kar_token_child_count(token)) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get_last(token, 0)->cursor, 1, "Неожиданный конец строки. Ожидалось имя метода.");
		return false;
	}
	
	KarToken* methodParams = kar_token_child_get(token, CHILD_INDEX);
	if (methodParams->type != KAR_TOKEN_SIGN_CALL_METHOD) {
        kar_project_error_list_create_add(errors, moduleName, &methodParams->cursor, 1, "Некорректное имя метода. Ожидалось, что будет идентификатор.");
		return false;
	}
	if (kar_token_child_count(methodParams) < 1) {
        kar_project_error_list_create_add(errors, moduleName, &methodParams->cursor, 1, "Не возможно найти имя метода.");
		return false;
	}
	
	KarToken* methodName = kar_token_child_get(methodParams, 0);
	if (methodName->type != KAR_TOKEN_IDENTIFIER) {
        kar_project_error_list_create_add(errors, moduleName, &methodName->cursor, 1, "Не корректное имя метода.");
		return false;
	}
	
	token->type = KAR_TOKEN_METHOD;
	token->str = kar_string_create(methodName->str);
	kar_token_child_erase(methodParams, 0);
	
	/*if (methodParams->children.count > 0) {
		methodParams->cursor = kar_token_child(methodParams, 0)->cursor;
	} else {*/
		// TODO: Сделать функцию в cursor.c, которая бы по строке вычисляла положение курсора с учетом символов новой строки.
		methodParams->cursor.column +=  (int)kar_unicode_length(token->str);
	//}
	
	return true;
}

static bool parse_method_parameters(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	const size_t CHILD_INDEX = 1;
	KarToken* parameters = kar_token_child_tear(token, CHILD_INDEX);
	parameters->type = KAR_TOKEN_METHOD_PARAMETERS;
	KAR_FREE(parameters->str);
	parameters->str = NULL;
	kar_token_child_insert(token, parameters, 1);
	
	for (size_t i = 0; i < kar_token_child_count(parameters); i++) {
		KarToken* parameter = kar_token_child_get(parameters, i);
		if (kar_token_child_count(parameter) == 1) {
			KarToken* mul = kar_token_child_get(parameter, 0);
			// TODO: Убрать вариант с * (KAR_TOKEN_SIGN_MUL). Так как для изменяемого параметра используется неопределённость.
			if (mul->type != KAR_TOKEN_SIGN_MUL) {
				kar_project_error_list_create_add(errors, moduleName, &parameter->cursor, 1, "Некорректный параметр метода.");
				return false;
			}
			if (kar_token_child_count(mul) != 2) {
				kar_project_error_list_create_add(errors, moduleName, &parameter->cursor, 1, "Некорректный параметр метода.");
				return false;
			}
			parameter->type = KAR_TOKEN_METHOD_PARAMETER_VAR;
			kar_token_child_tear(parameter, 0);
			KarToken* type = kar_token_child_tear(mul, 0);
			KarToken* name = kar_token_child_tear(mul, 0);
			kar_token_child_add(parameter, type);
			kar_token_child_add(parameter, name);
			kar_token_free(mul);
			
		} else if (kar_token_child_count(parameter) == 2) {
			parameter->type = KAR_TOKEN_METHOD_PARAMETER_CONST;
		} else {
			kar_project_error_list_create_add(errors, moduleName, &parameter->cursor, 1, "Некорректный параметр метода.");
			return false;
		}
		
		if (kar_token_child_count(parameter) != 2) {
			kar_project_error_list_create_add(errors, moduleName, &parameter->cursor, 1, "Некорректный параметр метода.");
			return false;
		}
		KarToken* type = kar_token_child_get(parameter, 0);
		KarToken* name = kar_token_child_get(parameter, 1);
		if (!kar_token_is_type(type->type)) {
            kar_project_error_list_create_add(errors, moduleName, &type->cursor, 1, "Некорректный тип параметра метода.");
			return false;
		}
		if (!kar_token_is_name(name->type)) {
            kar_project_error_list_create_add(errors, moduleName, &name->cursor, 1, "Некорректное имя параметра метода.");
			return false;
		}
	}
	return true;
}

static bool parse_return_type(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	const size_t CHILD_INDEX = 2;
	if (CHILD_INDEX == kar_token_child_count(token)) {
		return false;
	}
	KarToken* returnType = kar_token_create();
	returnType->type = KAR_TOKEN_METHOD_RETURN_TYPE;
	returnType->cursor = kar_token_child_get(token, CHILD_INDEX)->cursor;
	// TODO: Надо проверять корректность типа возвращаемого значения. Это либо идентификатор, либо стандартный тип.
	if (kar_token_child_get(token, CHILD_INDEX)->type != KAR_TOKEN_SIGN_COLON) {
		KarToken* returnTypeToken = kar_token_child_tear(token, CHILD_INDEX);
		kar_token_child_add(returnType, returnTypeToken);
		if (!kar_token_is_type(returnTypeToken->type)) {
            kar_project_error_list_create_add(errors, moduleName, &returnTypeToken->cursor, 1, "Некорректный тип возвращаемого значения функции.");
			return false;
		}
	}
	kar_token_child_insert(token, returnType, CHILD_INDEX);
	return true;
}

static bool parse_colon(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	const size_t CHILD_INDEX = 3;
	if (CHILD_INDEX == kar_token_child_count(token)) {
		// TODO: Надо указать курсор на конец токена.
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX - 1)->cursor, 1, "Пропущено двоеточие в конце строки.");
		return false;
	}
	if (kar_token_child_get(token, CHILD_INDEX)->type != KAR_TOKEN_SIGN_COLON) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Ожидался символ \":\".");
		return false;
	}
	if ((CHILD_INDEX + 1) == kar_token_child_count(token)) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Неожиданное выражение в конце строки.");
		return false;
	}
	kar_token_child_erase(token, CHILD_INDEX);
	return true;
}

static bool parse_algorithm(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	const size_t CHILD_INDEX = 3;
	if (kar_token_child_count(token) == CHILD_INDEX) {
		// TODO: Надо указать курсор на конец токена.
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX - 1)->cursor, 1, "Отсутствует алгоритм метода.");
		return false;
	}
	if (kar_token_child_count(token) > CHILD_INDEX + 1) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX + 1)->cursor, 1, "Слишком много алгоритмов или других элементов в методе.");
		return false;
	}
	
	// TODO: Возможно эти 2 блока если на самом деле являются частью проверки алгоритма. Надо перенести туда.
	if (kar_token_child_get(token, CHILD_INDEX)->type != KAR_TOKEN_BLOCK_BODY) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Здесь ожидалось найти тело метода.");
		return false;
	}
	if (kar_token_child_count(kar_token_child_get(token, CHILD_INDEX)) == 0) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "В теле метода должна быть хотя бы одна команда.");
		return false;
	}
	
    return kar_parser_parse_algorithm(kar_token_child_get(token, CHILD_INDEX), moduleName, errors);
}

// ----------------------------------------------------------------------------

KarParserStatus kar_parser_make_method(KarToken* token, KarString* moduleName, KarProjectErrorList* errors)
{
	size_t methodPos = kar_token_child_find(token, KAR_TOKEN_METHOD);
	if (methodPos == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
    if (!parse_modifiers(token, methodPos, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	methodPos = 1;
	
	parse_keyword(token);
	
    if (!parse_method_name(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!parse_method_parameters(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!parse_return_type(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!parse_colon(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!parse_algorithm(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	return KAR_PARSER_STATUS_PARSED;
}
