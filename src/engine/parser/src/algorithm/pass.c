/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_empty_block(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	// TODO: Написать тесты на команду пропустить.
	// TODO: Заменить токен INDENT на токен COMMAND_PASS.
	if (kar_token_child_count(token) != 1) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	KarToken* command = kar_token_child_get(token, 0);
	
	size_t tokenNum = kar_token_child_find(command, KAR_TOKEN_COMMAND_PASS);
	if (tokenNum == kar_token_child_count(command)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	if (tokenNum != 0) {
        kar_project_error_list_create_add(errors, moduleName, &command->cursor, 1, "Неизвестные токены перед командой пропустить.");
		return KAR_PARSER_STATUS_ERROR;
	}
	if (kar_token_child_count(command) > 1) {
        kar_project_error_list_create_add(errors, moduleName, &kar_token_child_get(command, 1)->cursor, 1, "Неизвестные токены после команды пропустить.");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	KarToken* keyword = kar_token_child_get(command, 0);
	command->type = keyword->type;
	command->cursor = keyword->cursor;
	kar_token_set_str(command, NULL);
	kar_token_child_erase(command, 0);
	
	return KAR_PARSER_STATUS_PARSED;
}
