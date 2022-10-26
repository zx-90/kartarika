/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_algo_expression(KarToken* token);
KarParserStatus kar_parser_make_return(KarToken* token, KarArray* errors);
KarParserStatus kar_parser_make_declaration(KarToken* token, KarArray* errors);
KarParserStatus kar_parser_make_assign(KarToken* token, KarArray* errors);

KarParserStatus kar_parser_make_block(KarToken* token, KarArray* errors);
KarParserStatus kar_parser_make_empty_block(KarToken* token, KarArray* errors);
KarParserStatus kar_parser_make_clean(KarToken* parent, size_t commandNum, KarArray* errors);
KarParserStatus kar_parser_make_if(KarToken* parent, size_t commandNum, KarArray* errors);
KarParserStatus kar_parser_make_while(KarToken* token, KarArray* errors);
KarParserStatus kar_parser_make_break(KarToken* token, KarArray* errors);

bool kar_parser_parse_command(KarToken* parent, size_t commandNum, KarArray* errors) {
	KarParserStatus status;
	KarToken* token = kar_token_child(parent, commandNum);
	
	status = kar_parser_make_algo_expression(token);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_return(token, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_declaration(token, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_assign(token, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_block(token, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_clean(parent, commandNum, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_if(parent, commandNum, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_while(token, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	status = kar_parser_make_break(token, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	kar_module_error_create_add(errors, &token->cursor, 1, "Неизвестная команда алгоритма.");
	return false;
}

bool kar_parser_parse_algorithm(KarToken* token, KarArray* errors) {
	KarParserStatus status;
	
	status = kar_parser_make_empty_block(token, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	bool b = true;
	for (size_t i = 0; i < token->children.count; ++i) {
		b = kar_parser_parse_command(token, i, errors) && b;
	}
	return b;
}
