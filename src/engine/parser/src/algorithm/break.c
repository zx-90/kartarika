/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_break(KarToken* token, KarProjectErrorList* errors) {
	size_t breakPos = kar_token_child_find(token, KAR_TOKEN_COMMAND_BREAK);
	size_t continuePos = kar_token_child_find(token, KAR_TOKEN_COMMAND_CONTINUE);
	
	if (breakPos == kar_token_child_count(token) && continuePos == kar_token_child_count(token)) {
		return KAR_PARSER_STATUS_NOT_PARSED;
	}
	
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (child->type == KAR_TOKEN_COMMAND_BREAK) {
			continue;
		}
		if (child->type == KAR_TOKEN_COMMAND_CONTINUE) {
			if (i == kar_token_child_count(token) - 1) {
				continue;
			}
			KarToken* nextChild = kar_token_child_get(token, i + 1);
			kar_project_error_list_create_add(errors, &nextChild->cursor, 1, "Лишнее выражение после ключевого слова \"следующий\".");
			return KAR_PARSER_STATUS_ERROR;
		}
		kar_project_error_list_create_add(errors, &child->cursor, 1, "Здесь ожидалось ключевое слово \"прервать\" или \"следующий\".");
		return KAR_PARSER_STATUS_ERROR;
	}
	
	token->type = KAR_TOKEN_COMMAND_BREAK;
	token->cursor = kar_token_child_get(token, 0)->cursor;
	kar_token_set_str(token, NULL);
	
	return KAR_PARSER_STATUS_PARSED;
}
