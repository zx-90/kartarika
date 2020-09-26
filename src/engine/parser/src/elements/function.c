/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

bool kar_parser_make_function(KarToken* token)
{
	for (size_t i = 0; i < token->children_count; ++i) {
		KarToken* child = token->children[i];
		size_t func;
		for (func = 0; func < child->children_count; ++func) {
			if (child->children[func]->type == KAR_TOKEN_FUNCTION) {
				break;
			}
		}
		if (func == child->children_count) {
			continue;
		}

		KarToken* modifiers = kar_token_create();
		modifiers->type = KAR_TOKEN_FUNC_MODIFIER;
		modifiers->cursor = child->children[0]->cursor;
		kar_token_insert_child(child, modifiers, 0);
		for (size_t i = 1; i < func + 1; ++i) {
			KarToken* modifier = kar_token_tear_child(child, 1);
			kar_token_add_child(modifiers, modifier);
		}
		func = 1;

		size_t funcName = func + 1;
		if (funcName == child->children_count) {
			continue;
		}
		if ( child->children[funcName]->type != KAR_TOKEN_IDENTIFIER ) {
			continue;
		}
		
		child->type = KAR_TOKEN_FUNCTION;
		child->str = child->children[funcName]->str;
		kar_token_erase_child(child, func);
		
		size_t signColon = func + 1;
		size_t returnKeyword = func + 1;
		if (returnKeyword != child->children_count) {
			if (child->children[returnKeyword]->type != KAR_TOKEN_FUNC_RETURN_TYPE) {
				return false;
			}
			size_t returnType = returnKeyword + 1;
			if (returnType == child->children_count) {
				return false;
			}
			KarToken* returnTypeToken = kar_token_tear_child(child, returnType);
			kar_token_add_child(child->children[returnKeyword], returnTypeToken);
			signColon += 1;
		}
		
		if ( signColon == child->children_count ) {
			return false;
		}
		if (child->children[signColon]->type != KAR_TOKEN_SIGN_COLON) { // проверить есть ли тело функции.
			return false;
		}
		if ((signColon + 1) == child->children_count) {
			return false;
		}
		kar_token_erase_child(child, signColon);
	}
	return true;
}
