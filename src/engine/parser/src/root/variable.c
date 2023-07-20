/* Copyright © 2021-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/string.h"
#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

static bool parse_modifiers(KarToken* token, size_t variable, KarString* moduleName, KarProjectErrorList* errors)
{
	KarToken* modifiers = kar_token_create_fill(KAR_TOKEN_VAR_MODIFIER, kar_token_child_get(token, 0)->cursor, NULL);
	kar_token_child_insert(token, modifiers, 0);
	kar_token_child_move_to_end(token, modifiers, 1, variable);
	
	bool static_modifier = false;
	bool inherit_modifier = false;
	for (size_t i = 0; i < kar_token_child_count(modifiers); i++) {
		KarToken* child = kar_token_child_get(modifiers, i);
		if (
			child->type != KAR_TOKEN_MODIFIER_STAT &&
			child->type != KAR_TOKEN_VAR_MODIFIER_DYNAMIC &&
			child->type != KAR_TOKEN_VAR_MODIFIER_PRIVATE &&
			child->type != KAR_TOKEN_VAR_MODIFIER_PROTECTED &&
			child->type != KAR_TOKEN_VAR_MODIFIER_PUBLIC
		) {
            kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Некорректный модификатор при объявлении поля.");
			return false;
		}
		if (
			child->type == KAR_TOKEN_MODIFIER_STAT ||
			child->type == KAR_TOKEN_VAR_MODIFIER_DYNAMIC
		) {
			if (static_modifier) {
                kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Объявлено более одного модификатора статичности при инициализации переменной.");
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
                kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Объявлено более одного модификатора области видимости при инициализации переменной.");
				return false;
			} else {
				inherit_modifier = true;
			}
		}
	}

	return true;
}

static void parse_keyword(KarToken* token) {
	const size_t CHILD_INDEX = 1;
	kar_token_child_erase(token, CHILD_INDEX);
}

static bool parse_const_name(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	const size_t CHILD_INDEX = 1;
	if (CHILD_INDEX == kar_token_child_count(token)) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Отсутствует имя переменной.");
		return false;
	}
	if (kar_token_child_get(token, CHILD_INDEX)->type != KAR_TOKEN_IDENTIFIER || kar_token_child_count(kar_token_child_get(token, CHILD_INDEX)) > 0) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Имя переменной должно быть идентификатором.");
		return false;
	}
	token->type = KAR_TOKEN_FIELD_CONST;
	token->str = kar_string_create(kar_token_child_get(token, CHILD_INDEX)->str);
	kar_token_child_erase(token, CHILD_INDEX);
	return true;
}

static bool parse_var_name(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	const size_t CHILD_INDEX = 1;
	if (CHILD_INDEX == kar_token_child_count(token)) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Отсутствует имя переменной.");
		return false;
	}
	if (kar_token_child_get(token, CHILD_INDEX)->type != KAR_TOKEN_IDENTIFIER || kar_token_child_count(kar_token_child_get(token, CHILD_INDEX)) > 0) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Имя переменной должно быть идентификатором.");
		return false;
	}
	token->type = KAR_TOKEN_FIELD_VAR;
	token->str = kar_string_create(kar_token_child_get(token, CHILD_INDEX)->str);
	kar_token_child_erase(token, CHILD_INDEX);
	return true;
}

static bool parse_equation_sign(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	size_t CHILD_INDEX = 1;
	if (CHILD_INDEX == kar_token_child_count(token)) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Неожиданный конец присвоения. Переменная должны быть инициализирована.");
		return false;
	}
	if (kar_token_child_get(token, CHILD_INDEX)->type != KAR_TOKEN_SIGN_ASSIGN) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Переменная должна быть инициализирована.");
		return false;
	}
	kar_token_child_erase(token, CHILD_INDEX);
	return true;
}

static bool parse_expression(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	size_t CHILD_INDEX = 1;
	if (CHILD_INDEX == kar_token_child_count(token)) {
		// TODO: Надо положение курсора в конце строки вычислять.
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX - 1)->cursor, 1, "Неожиданный конец присвоения. Отсутствует правая часть присвоения.");
		return false;
	}
	if (kar_token_child_count(token) > CHILD_INDEX + 1) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX + 1)->cursor, 1, "Слишком много выражений в правой части присвоения.");
		return false;
	}
	if (!kar_parser_is_expression(kar_token_child_get(token, CHILD_INDEX)->type)) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, CHILD_INDEX)->cursor, 1, "Некорректное выражение в правой части присвоения.");
		return false;
	}
	KarToken* body = kar_token_create_fill(KAR_TOKEN_BLOCK_BODY, kar_token_child_get(token, CHILD_INDEX)->cursor, NULL);
	kar_token_child_add(token, body);
	kar_token_child_move_to_end(token, body, CHILD_INDEX, 1);
	return true;
}

// ----------------------------------------------------------------------------

KarParserStatus kar_parser_make_constant(KarToken* token, KarString* moduleName, KarProjectErrorList* errors)
{
	size_t variable = kar_token_child_find(token, KAR_TOKEN_FIELD_CONST);
	if (variable == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
    if (!parse_modifiers(token, variable, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	variable = 1;
	
	parse_keyword(token);
	
    if (!parse_const_name(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!parse_equation_sign(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!parse_expression(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	return KAR_PARSER_STATUS_PARSED;
}

KarParserStatus kar_parser_make_variable(KarToken* token, KarString* moduleName, KarProjectErrorList* errors)
{
	size_t variable = kar_token_child_find(token, KAR_TOKEN_FIELD_VAR);
	if (variable == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
    if (!parse_modifiers(token, variable, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	variable = 1;
	
	parse_keyword(token);
	
    if (!parse_var_name(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!parse_equation_sign(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
    if (!parse_expression(token, moduleName, errors)) {
		return KAR_PARSER_STATUS_ERROR;
	}
	
	return KAR_PARSER_STATUS_PARSED;
}
