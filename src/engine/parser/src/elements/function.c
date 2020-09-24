/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

bool k_parser_make_function(KToken* token)
{
	for (size_t i = 0; i < token->children_count; ++i) {
		KToken* child = token->children[i];
		size_t func;
		for (func = 0; func < child->children_count; ++func) {
			if (child->children[func]->type == TOKEN_FUNCTION) {
				break;
			}
		}
		if (func == child->children_count) {
			continue;
		}

		KToken* modifiers = k_token_create();
		modifiers->type = TOKEN_FUNC_MODIFIER;
		modifiers->cursor = child->children[0]->cursor;
		k_token_insert_child(child, modifiers, 0);
		for (size_t i = 1; i < func + 1; ++i) {
			KToken* modifier = k_token_tear_child(child, 1);
			k_token_add_child(modifiers, modifier);
		}
		func = 1;

		size_t funcName = func + 1;
		if (funcName == child->children_count) {
			continue;
		}
		if ( child->children[funcName]->type != TOKEN_IDENTIFIER ) {
			continue;
		}
		
		child->type = TOKEN_FUNCTION;
		child->str = child->children[funcName]->str;
		k_token_erase_child(child, func);
		
		size_t signColon = func + 1;
		size_t returnKeyword = func + 1;
		if (returnKeyword != child->children_count) {
			if (child->children[returnKeyword]->type != TOKEN_FUNC_RETURN_TYPE) {
				return false;
			}
			size_t returnType = returnKeyword + 1;
			if (returnType == child->children_count) {
				return false;
			}
			KToken* returnTypeToken = k_token_tear_child(child, returnType);
			k_token_add_child(child->children[returnKeyword], returnTypeToken);
			signColon += 1;
		}
		
		if ( signColon == child->children_count ) {
			return false;
		}
		if (child->children[signColon]->type != TOKEN_SIGN_COLON) { // проверить есть ли тело функции.
			return false;
		}
		if ((signColon + 1) == child->children_count) {
			return false;
		}
		k_token_erase_child(child, signColon);
	}
	return true;
}
