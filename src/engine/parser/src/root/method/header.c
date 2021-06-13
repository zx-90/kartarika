/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/string.h"
#include "core/token.h"

bool kar_parser_make_method(KarToken* token)
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

		size_t methodNamePos = methodPos + 1;
		if (methodNamePos == child->children.count) {
			continue;
		}
		if (kar_token_child(child, methodNamePos)->type != KAR_TOKEN_IDENTIFIER ) {
			continue;
		}
		
		// TODO: Проверить, что в methodNamePos нет параметров, и перенести его в токен с параметрами.
		child->type = KAR_TOKEN_METHOD;
		child->str = kar_string_create_copy(kar_token_child(child, methodNamePos)->str);

		KarToken* parameters = kar_token_create();
		parameters->type = KAR_TOKEN_METHOD_PARAMETERS;
		parameters->cursor = kar_token_child(child, methodNamePos)->cursor;
		kar_token_child_insert(child, parameters, 1);
		
		kar_token_child_erase(child, methodNamePos);
		kar_token_child_erase(child, methodNamePos);
		
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
