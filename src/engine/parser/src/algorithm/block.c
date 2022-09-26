/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_block(KarToken* token) {
	if (token->type != KAR_TOKEN_INDENT) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	if (token->children.count > 0 && kar_token_child(token, 0)->type != KAR_TOKEN_COMMAND_BLOCK) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	// TODO: Доделать.
	
	return KAR_PARSER_STATUS_PARSED;
}
