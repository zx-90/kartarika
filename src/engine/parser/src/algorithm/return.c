/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_return(KarToken* token, KarArray* errors) {
	size_t returnPos = kar_token_child_find(token, KAR_TOKEN_METHOD_RETURN);
	
	if (returnPos == token->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	KarToken* returnToken = kar_token_child(token, returnPos);
	if (returnPos != 0) {
		kar_module_error_create_add(errors, &returnToken->cursor, 1, "Ключевое слово вернуть должно стоять в начале команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	// Вернуть без параментра.
	if (token->children.count == 1) {
		return KAR_PARSER_STATUS_PARSED;
	}
	
	KarToken* expression = kar_token_child(token, 1);
	if (!kar_parser_is_expression(expression->type)) {
		kar_module_error_create_add(errors, &expression->cursor, 1, "Здесь ожидалось выражение.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	if (token->children.count > 2) {
		kar_module_error_create_add(errors, &kar_token_child(token, 2)->cursor, 1, "Здесь ожидался конец команды.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	kar_token_child_move_to_end(token, returnToken, 1, 1);
	
	return KAR_PARSER_STATUS_PARSED;
}
