/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/string.h"
#include "core/token.h"

bool kar_parser_make_function(KarToken* token)
{
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		size_t func;
		for (func = 0; func < child->children.count; ++func) {
			if (kar_token_child(child, func)->type == KAR_TOKEN_FUNCTION) {
				break;
			}
		}
		if (func == child->children.count) {
			continue;
		}

		KarToken* modifiers = kar_token_create();
		modifiers->type = KAR_TOKEN_FUNC_MODIFIER;
		modifiers->cursor = kar_token_child(child, 0)->cursor;
		kar_token_child_insert(child, modifiers, 0);
		kar_token_child_move_to_end(child, modifiers, 1, func);
		func = 1;

		size_t funcName = func + 1;
		if (funcName == child->children.count) {
			continue;
		}
		if (kar_token_child(child, funcName)->type != KAR_TOKEN_IDENTIFIER ) {
			continue;
		}
		
		// TODO: Проверить, что в funcName нет параметров, и перенести его в токен с параметрами.
		child->type = KAR_TOKEN_FUNCTION;
		child->str = kar_string_create_copy(kar_token_child(child, funcName)->str);

		KarToken* parameters = kar_token_create();
		parameters->type = KAR_TOKEN_FUNC_PARAMETERS;
		parameters->cursor = kar_token_child(child, funcName)->cursor;
		kar_token_child_insert(child, parameters, 1);
		
		kar_token_child_erase(child, funcName);
		kar_token_child_erase(child, funcName);
		
		size_t signColon = func + 1;
		size_t returnKeyword = func + 1;
		if (returnKeyword != child->children.count) {
			if (kar_token_child(child, returnKeyword)->type != KAR_TOKEN_FUNC_RETURN_TYPE) {
				return false;
			}
			size_t returnType = returnKeyword + 1;
			if (returnType == child->children.count) {
				return false;
			}
			KarToken* returnTypeToken = kar_token_child_tear(child, returnType);
			kar_token_child_add(kar_token_child(child, returnKeyword), returnTypeToken);
			signColon += 1;
		}
		
		if ( signColon == child->children.count ) {
			return false;
		}
		if (kar_token_child(child, signColon)->type != KAR_TOKEN_SIGN_COLON) { // проверить есть ли тело функции.
			return false;
		}
		if ((signColon + 1) == child->children.count) {
			return false;
		}
		kar_token_child_erase(child, signColon);
	}
	return true;
}
