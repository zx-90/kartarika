/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_empty_block(KarToken* token, KarArray* errors) {
	// TODO: Написать тесты на команду пропустить.
	// TODO: Заменить токен INDENT на токен COMMAND_PASS.
	if (token->children.count != 1) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	KarToken* command = kar_token_child(token, 0);
	
	size_t tokenNum = kar_token_child_find(command, KAR_TOKEN_COMMAND_PASS);
	if (tokenNum == command->children.count) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	if (tokenNum != 0) {
		kar_module_error_create_add(errors, &command->cursor, 1, "Неизвестные токены перед командой пропустить.");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (command->children.count > 1) {
		kar_module_error_create_add(errors, &kar_token_child(command, 1)->cursor, 1, "Неизвестные токены после команды пропустить.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	KarToken* keyword = kar_token_child(command, 0);
	command->type = keyword->type;
	command->cursor = keyword->cursor;
	kar_token_set_str(command, NULL);
	kar_token_child_erase(command, 0);
	
	return KAR_PARSER_STATUS_PARSED;
}
