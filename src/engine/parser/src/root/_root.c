/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_variable(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_constant(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_method(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);

static void erase_middle_brackets(KarToken* token) {
	while (kar_token_child_count(token) == 1 && kar_token_child_get(token, 0)->type == KAR_TOKEN_SIGN_OPEN_BRACES) {
		KarToken* brackets = kar_token_child_tear(token, 0);
		while (kar_token_child_count(brackets) > 0) {
			KarToken* grandChild = kar_token_child_tear(brackets, 0);
			kar_token_child_add(token, grandChild);
		}
		kar_token_free(brackets);
	}
}

static bool parse_line(KarToken* child, KarString* moduleName, KarProjectErrorList* errors) {
	KarParserStatus status;
	
	erase_middle_brackets(child);
	
    status = kar_parser_make_variable(child, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_constant(child, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status =  kar_parser_make_method(child, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Неизвестный корневой элемент. Необходимо объявление поля, константы или метода");
	return false;
}

bool kar_parser_parse_root(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	bool b = true;
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
        b = parse_line(child, moduleName, errors) && b;
	}
	return b;
}
