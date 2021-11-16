/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/alloc.h"
#include "core/string.h"
#include "core/token.h"
#include "core/module_error.h"

static bool kar_token_is_name(KarToken* token) {
	return
		token->type == KAR_TOKEN_IDENTIFIER;
	// TODO: добавить ещё тип класса. То есть проверить, что это путь.
}

static bool kar_token_is_type(KarToken* token) {
	return
		token->type == KAR_TOKEN_VAR_BOOL ||
		token->type == KAR_TOKEN_VAR_FLOAT32 ||
		token->type == KAR_TOKEN_VAR_FLOAT64 ||
		token->type == KAR_TOKEN_VAR_FLOAT80 ||
		token->type == KAR_TOKEN_VAR_INTEGER8 ||
		token->type == KAR_TOKEN_VAR_INTEGER16 ||
		token->type == KAR_TOKEN_VAR_INTEGER32 ||
		token->type == KAR_TOKEN_VAR_INTEGER64 ||
		token->type == KAR_TOKEN_VAR_UNSIGNED8 ||
		token->type == KAR_TOKEN_VAR_UNSIGNED16 ||
		token->type == KAR_TOKEN_VAR_UNSIGNED32 ||
		token->type == KAR_TOKEN_VAR_UNSIGNED64 ||
		token->type == KAR_TOKEN_VAR_STRING;
	// TODO: добавить ещё тип класса. То есть проверить, что это путь.
}

bool kar_parser_make_method(KarToken* token, KarArray* errors)
{
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		size_t methodPos;
		for (methodPos = 0; methodPos < child->children.count; ++methodPos) {
			if (kar_token_child(child, methodPos)->type == KAR_TOKEN_METHOD) {
				break;
			}
		}
		if (methodPos == child->children.count) {
			continue;
		}

		KarToken* modifiers = kar_token_create();
		modifiers->type = KAR_TOKEN_METHOD_MODIFIER;
		modifiers->cursor = kar_token_child(child, 0)->cursor;
		kar_token_child_insert(child, modifiers, 0);
		kar_token_child_move_to_end(child, modifiers, 1, methodPos);
		methodPos = 1;
		
		bool static_modifier = false;
		bool inherit_modifier = false;
		bool finalize_modifier = false;
		bool overload_modifier = false;
		for (size_t i = 0; i < modifiers->children.count; i++){
			KarToken* child = kar_token_child(modifiers, i);
			if (
				child->type != KAR_TOKEN_MODIFIER_STAT &&
				child->type != KAR_TOKEN_METHOD_MODIFIER_DYNAMIC &&
				child->type != KAR_TOKEN_METHOD_MODIFIER_PRIVATE &&
				child->type != KAR_TOKEN_METHOD_MODIFIER_PROTECTED &&
				child->type != KAR_TOKEN_METHOD_MODIFIER_PUBLIC &&
				child->type != KAR_TOKEN_METHOD_MODIFIER_FINALIZED &&
				child->type != KAR_TOKEN_METHOD_MODIFIER_INHERITED &&
				child->type != KAR_TOKEN_METHOD_MODIFIER_OVERLOAD
			) {
				kar_module_error_create_add(errors, &child->cursor, 1, "Некорректный модификатор при объявлении функции.");
				return false;
			}
			if (
				child->type == KAR_TOKEN_MODIFIER_STAT ||
				child->type == KAR_TOKEN_METHOD_MODIFIER_DYNAMIC
			) {
				if (static_modifier) {
					kar_module_error_create_add(errors, &child->cursor, 1, "Объявлено более одного модификатора статичности при объявлении функции.");
					return false;
				} else {
					static_modifier = true;
				}
			}
			if (
				child->type == KAR_TOKEN_METHOD_MODIFIER_PRIVATE ||
				child->type == KAR_TOKEN_METHOD_MODIFIER_PROTECTED ||
				child->type == KAR_TOKEN_METHOD_MODIFIER_PUBLIC
			) {
				if (inherit_modifier) {
					kar_module_error_create_add(errors, &child->cursor, 1, "Объявлено более одного модификатора области видимости при объявлении функции.");
					return false;
				} else {
					inherit_modifier = true;
				}
			}
			if (
				child->type == KAR_TOKEN_METHOD_MODIFIER_FINALIZED ||
				child->type == KAR_TOKEN_METHOD_MODIFIER_INHERITED
			) {
				if (finalize_modifier) {
					kar_module_error_create_add(errors, &child->cursor, 1, "Объявлено более одного модификатора финализации при объявлении функции.");
					return false;
				} else {
					finalize_modifier = true;
				}
			}
			if (
				child->type == KAR_TOKEN_METHOD_MODIFIER_OVERLOAD
			) {
				if (overload_modifier) {
					kar_module_error_create_add(errors, &child->cursor, 1, "Объявлено более одного модификатора перегрузки при объявлении функции.");
					return false;
				} else {
					overload_modifier = true;
				}
			}
		}
		
		size_t methodNamePos = methodPos + 1;
		if (methodNamePos == child->children.count) {
			kar_module_error_create_add(errors, &kar_token_child(child, child->children.count - 1)->cursor, 1, "Неожиданный конец строки. Ожидалось имя метода.");
			return false;
		}
		if (kar_token_child(child, methodNamePos)->type != KAR_TOKEN_IDENTIFIER ) {
			kar_module_error_create_add(errors, &kar_token_child(child, methodNamePos)->cursor, 1, "Некорректное имя метода. Ожидалось, что будет идентификатор.");
			return false;
		}
		
		// TODO: Проверить, что в methodNamePos нет параметров, и перенести его в токен с параметрами.
		child->type = KAR_TOKEN_METHOD;
		child->str = kar_string_create_copy(kar_token_child(child, methodNamePos)->str);

		KarToken* methodName = kar_token_child(child, methodNamePos);
		if (methodName->children.count < 1) {
			kar_module_error_create_add(errors, &methodName->cursor, 1, "Отсутствуют параметры метода.");
			return false;
		}
		KarToken* parameters = kar_token_child_tear(methodName, 0);
		parameters->type = KAR_TOKEN_METHOD_PARAMETERS;
		KAR_FREE(parameters->str)
		parameters->str = NULL;
		kar_token_child_insert(child, parameters, 1);
		
		kar_token_child_erase(child, methodNamePos);
		kar_token_child_erase(child, methodNamePos);
		
		if (parameters->children.count == 0) {
			
		} else if (
			parameters->children.count == 2 &&
			kar_token_is_type(kar_token_child(parameters, 0)) &&
			kar_token_is_name(kar_token_child(parameters, 1))
		) {
			KarToken* parameter = kar_token_create();
			parameter->type = KAR_TOKEN_METHOD_PARAMETER_CONST;
			parameter->cursor = kar_token_child(parameters, 0)->cursor;
			kar_token_child_add(parameter, kar_token_child_tear(parameters, 0));
			kar_token_child_add(parameter, kar_token_child_tear(parameters, 0));
			kar_token_child_add(parameters, parameter);
		} else if (
			parameters->children.count == 3 &&
			kar_token_is_type(kar_token_child(parameters, 0)) &&
			kar_token_child(parameters, 1)->type == KAR_TOKEN_SIGN_MUL &&
			kar_token_is_name(kar_token_child(parameters, 2))
		) {
			KarToken* parameter = kar_token_create();
			parameter->type = KAR_TOKEN_METHOD_PARAMETER_VAR;
			parameter->cursor = kar_token_child(parameters, 0)->cursor;
			kar_token_child_add(parameter, kar_token_child_tear(parameters, 0));
			kar_token_child_erase(parameters, 0);
			kar_token_child_add(parameter, kar_token_child_tear(parameters, 0));
			kar_token_child_add(parameters, parameter);
		} else {
			kar_module_error_create_add(errors, &parameters->cursor, 1, "Не корректные параметры метода.");
			return false;			
		}
		
		size_t signColonPos = methodPos + 1;
		size_t returnTypePos = methodPos + 1;
		if (returnTypePos == child->children.count) {
			return false;
		// TODO: Надо проверять корректность типа возвращаемого знакчения. Это либо идентификатор, либо стандартный тип.
		} else if (kar_token_child(child, signColonPos)->type != KAR_TOKEN_SIGN_COLON) {
			KarToken* returnType = kar_token_create();
			returnType->type = KAR_TOKEN_METHOD_RETURN_TYPE;
			returnType->cursor = kar_token_child(child, returnTypePos)->cursor;
			KarToken* returnTypeToken = kar_token_child_tear(child, returnTypePos);
			kar_token_child_add(returnType, returnTypeToken);
			kar_token_child_insert(child, returnType, returnTypePos);
			//kar_token_child_move_to_end(child, returnType, 1, methodPos);
			signColonPos += 1;
		}
		
		if ( signColonPos == child->children.count ) {
			return false;
		}
		if (kar_token_child(child, signColonPos)->type != KAR_TOKEN_SIGN_COLON) { // проверить есть ли тело метода.
			return false;
		}
		if ((signColonPos + 1) == child->children.count) {
			return false;
		}
		kar_token_child_erase(child, signColonPos);
	}
	return true;
}