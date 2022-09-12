/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"

// TODO: Проработать проверку на краткое имя, полное имя (путь) для выражений и полное имя для типов.
bool kar_token_is_name(KarToken* token) {
    return
        token->type == KAR_TOKEN_IDENTIFIER &&
        token->children.count == 0;
    // TODO: добавить ещё тип класса. То есть проверить, что это путь.
}

bool kar_token_is_type(KarToken* token) {
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
        token->type == KAR_TOKEN_VAR_STRING ||
        token->type == KAR_TOKEN_IDENTIFIER;
	// TODO: добавить ещё тип класса. То есть проверить, что это путь.
}

bool kar_parser_is_expression(KarToken* token)
{
	// TODO: Написать функцию и добавить тесты. Проверка на то,
	//       является ли токен вместе со всеми потомками корректным выражением.
	KarTokenType type = token->type;
	return type != KAR_TOKEN_MODIFIER_STAT;
}