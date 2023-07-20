/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/stack.h"
#include "model/project_error_list.h"
#include "model/token.h"

static bool extern_bracket(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	KarStack* stack = kar_stack_create(kar_token_child_count(token));
	
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		if (kar_token_child_get(token, i)->type == KAR_TOKEN_SIGN_OPEN_BRACES) {
			kar_stack_push(stack, i);
		}
		if (kar_token_child_get(token, i)->type == KAR_TOKEN_SIGN_CLOSE_BRACES) {
			if (kar_stack_is_empty(stack)) {
                kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, i)->cursor, 1, "У закрывающейся скобки нет соответствующей ей открывающейся.");
				kar_stack_free(stack);
				return false;
			}
			size_t open = kar_stack_pop(stack);
			kar_token_child_move_to_end(token, kar_token_child_get(token, open), open + 1, i - open - 1);
			kar_token_child_erase(token, open + 1);
			i = open;
		}
	}
	
	if (!kar_stack_is_empty(stack)) {
		while (!kar_stack_is_empty(stack)) {
            kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(token, kar_stack_pop(stack))->cursor, 1, "У открывающейся скобки нет соответствующей ей закрывающейся.");
		}
		kar_stack_free(stack);
		return false;
	}
	
	kar_stack_free(stack);
	return true;
}

static bool foreach(KarToken* token, KarString* moduleName, KarProjectErrorList* errors)
{
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
        if (!foreach(kar_token_child_get(token, i), moduleName, errors)) {
			return false;
		}
	}
    return extern_bracket(token, moduleName, errors);
}

bool kar_parser_extern_brackets(KarToken* token, KarString* moduleName, KarProjectErrorList* errors)
{
    return foreach(token, moduleName, errors);
}
