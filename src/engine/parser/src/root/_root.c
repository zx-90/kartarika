/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"

#include "parser/base.h"
//#include "parser/root.h"

KarParserStatus kar_parser_make_variable(KarToken* token, KarArray* errors);
KarParserStatus kar_parser_make_constant(KarToken* token, KarArray* errors);
KarParserStatus kar_parser_make_method(KarToken* token, KarArray* errors);

static void erase_middle_brackets(KarToken* token) {
	while (token->children.count == 1 && kar_token_child(token, 0)->type == KAR_TOKEN_SIGN_OPEN_BRACES) {
		KarToken* brackets = kar_token_child_tear(token, 0);
		while (brackets->children.count > 0) {
			KarToken* grandChild = kar_token_child_tear(brackets, 0);
			kar_token_child_add(token, grandChild);
		}
		kar_token_free(brackets);
	}
}

static bool parse_line(KarToken* child, KarArray* errors) {
	KarParserStatus status;
	
	erase_middle_brackets(child);
	
	status = kar_parser_make_variable(child, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_constant(child, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status =  kar_parser_make_method(child, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	kar_module_error_create_add(errors, &child->cursor, 1, "Неизвестный корневой элемент. Необходимо объявление поля, константы или метода");
	return false;
}

bool kar_parser_parse_root(KarToken* token, KarArray* errors) {
	bool b = true;
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		b = parse_line(child, errors) && b;
	}
	return b;
}
