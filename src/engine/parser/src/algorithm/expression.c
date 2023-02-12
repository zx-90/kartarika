/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_algo_expression(KarToken* token) {
	if (token->type == KAR_TOKEN_INDENT && (kar_token_child_count(token) == 1) && kar_parser_is_expression(kar_token_child_get(token, 0)->type)) {
		token->type = KAR_TOKEN_COMMAND_EXPRESSION;
		token->cursor =  kar_token_get_first_grandchild(token)->cursor;
		kar_token_set_str(token, NULL);
		return KAR_PARSER_STATUS_PARSED;
	}
	return KAR_PARSER_STATUS_NOT_PARSED;
}
