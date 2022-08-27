/* Copyright © 2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/string.h"
#include "core/token.h"
#include "core/module_error.h"

#include "parser/expression.h"

static size_t find_var_token(KarToken* token) {
	size_t result;
	for (result = 0; result < token->children.count; ++result) {
		if (kar_token_child(token, result)->type == KAR_TOKEN_FIELD_VAR) {
			return result;
		}
	}
	return result;
}

static bool collect_modifiers(KarToken* token, size_t variable, KarArray* errors)
{
	KarToken* modifiers = kar_token_create_fill(KAR_TOKEN_VAR_MODIFIER, kar_token_child(token, 0)->cursor, NULL);
	kar_token_child_insert(token, modifiers, 0);
	kar_token_child_move_to_end(token, modifiers, 1, variable);
	
	bool static_modifier = false;
	bool inherit_modifier = false;
	for (size_t i = 0; i < modifiers->children.count; i++) {
		KarToken* child = kar_token_child(modifiers, i);
		if (
			child->type != KAR_TOKEN_MODIFIER_STAT &&
			child->type != KAR_TOKEN_VAR_MODIFIER_DYNAMIC &&
			child->type != KAR_TOKEN_VAR_MODIFIER_PRIVATE &&
			child->type != KAR_TOKEN_VAR_MODIFIER_PROTECTED &&
			child->type != KAR_TOKEN_VAR_MODIFIER_PUBLIC
		) {
			kar_module_error_create_add(errors, &child->cursor, 1, "Некорректный модификатор при объявлении поля.");
			return false;
		}
		if (
			child->type == KAR_TOKEN_MODIFIER_STAT ||
			child->type == KAR_TOKEN_VAR_MODIFIER_DYNAMIC
		) {
			if (static_modifier) {
				kar_module_error_create_add(errors, &child->cursor, 1, "Объявлено более одного модификатора статичности при инициализации переменной.");
				return false;
			} else {
				static_modifier = true;
			}
		}
		if (
			child->type == KAR_TOKEN_VAR_MODIFIER_PRIVATE ||
			child->type == KAR_TOKEN_VAR_MODIFIER_PROTECTED ||
			child->type == KAR_TOKEN_VAR_MODIFIER_PUBLIC
		) {
			if (inherit_modifier) {
				kar_module_error_create_add(errors, &child->cursor, 1, "Объявлено более одного модификатора области видимости при инициализации переменной.");
				return false;
			} else {
				inherit_modifier = true;
			}
		}
	}

	return true;
}

bool kar_parser_make_variable(KarToken* token, KarArray* errors)
{
	// TODO: Добавить обработку константы.
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		
		size_t variable = find_var_token(child);
		if (variable == child->children.count) {
			continue;
		}
		
		if (!collect_modifiers(child, variable, errors)) {
			return false;
		}
		variable = 1;
		
		// Удаление ключевого слова "поле" или "конст".
		kar_token_child_erase(child, variable);

		size_t varName = variable;
		if (varName == child->children.count) {
			kar_module_error_create_add(errors, &kar_token_child(child, variable)->cursor, 1, "Отсутствует имя переменной.");
			return false;
		}
		if (kar_token_child(child, varName)->type != KAR_TOKEN_IDENTIFIER || kar_token_child(child, varName)->children.count > 0) {
			kar_module_error_create_add(errors, &kar_token_child(child, variable)->cursor, 1, "Имя переменной должно быть идентификатором.");
			return false;
		}
		child->type = KAR_TOKEN_FIELD_VAR;
		child->str = kar_string_create_copy(kar_token_child(child, varName)->str);
		kar_token_child_erase(child, variable);
		
		size_t varAssign = variable;
		if (varAssign == child->children.count) {
			kar_module_error_create_add(errors, &kar_token_child(child, variable)->cursor, 1, "Неожиданный конец присвоения. Переменная должны быть инициализирована.");
			return false;
		}
		if (kar_token_child(child, varAssign)->type != KAR_TOKEN_SIGN_ASSIGN) {
			kar_module_error_create_add(errors, &kar_token_child(child, variable)->cursor, 1, "Переменная должна быть инициализирована.");
			return false;
		}
		kar_token_child_erase(child, varAssign);
		
		size_t bodyNum = varAssign;
		if (bodyNum == child->children.count) {
			// TODO: Надо положение курсора в конце строки вычислять.
			kar_module_error_create_add(errors, &kar_token_child(child, bodyNum - 1)->cursor, 1, "Неожиданный конец присвоения. Отсутствует правая часть присвоения.");
			return false;
		}
		if (child->children.count > bodyNum + 1) {
			kar_module_error_create_add(errors, &kar_token_child(child, bodyNum + 1)->cursor, 1, "Слишком много выражений в правой части присвоения.");
			return false;
		}
		if (!kar_parser_is_expression(kar_token_child(child, bodyNum))) {
			kar_module_error_create_add(errors, &kar_token_child(child, bodyNum)->cursor, 1, "Некорректное выражение в правой части присвоения.");
			return false;
		}
		KarToken* body = kar_token_create_fill(KAR_TOKEN_BLOCK_BODY, kar_token_child(child, bodyNum)->cursor, NULL);
		kar_token_child_add(child, body);
		kar_token_child_move_to_end(child, body, bodyNum, 1);
	}
	return true;
}