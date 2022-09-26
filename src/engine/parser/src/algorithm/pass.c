/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"
#include "parser/base.h"

static size_t find_pass_token(KarToken* token) {
	size_t methodPos;
	for (methodPos = 0; methodPos < token->children.count; ++methodPos) {
		if (kar_token_child(token, methodPos)->type == KAR_TOKEN_COMMAND_PASS) {
			break;
		}
	}
	return methodPos;
}

KarParserStatus kar_parser_make_empty_block(KarToken* token, KarArray* errors) {
	if (token->children.count != 1) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	KarToken* command = kar_token_child(token, 0);
	
	size_t tokenNum = find_pass_token(command);
	if (tokenNum == command->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	if (tokenNum != 0) {
		kar_module_error_create_add(errors, &token->cursor, 1, "Неизвестные токены перед командой пропустить.");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (command->children.count > 1) {
		kar_module_error_create_add(errors, &kar_token_child(token, 1)->cursor, 1, "Неизвестные токены после команды пропустить.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	return KAR_PARSER_STATUS_PARSED;
}
