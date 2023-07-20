/* Copyright © 2022,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"
#include "model/project_error_list.h"
#include "parser/base.h"

KarParserStatus kar_parser_make_algo_expression(KarToken* token);
KarParserStatus kar_parser_make_return(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_declaration(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_assign(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);

KarParserStatus kar_parser_make_block(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_empty_block(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_clean(KarToken* parent, size_t commandNum, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_if(KarToken* parent, size_t commandNum, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_while(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);
KarParserStatus kar_parser_make_break(KarToken* token, KarString* moduleName, KarProjectErrorList* errors);

bool kar_parser_parse_command(KarToken* parent, size_t commandNum, KarString* moduleName, KarProjectErrorList* errors) {
	KarParserStatus status;
	KarToken* token = kar_token_child_get(parent, commandNum);
	
	status = kar_parser_make_algo_expression(token);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_return(token, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_declaration(token, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_assign(token, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_block(token, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_clean(parent, commandNum, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_if(parent, commandNum, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_while(token, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    status = kar_parser_make_break(token, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
    kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Неизвестная команда алгоритма.");
	return false;
}

bool kar_parser_parse_algorithm(KarToken* token, KarString* moduleName, KarProjectErrorList* errors) {
	KarParserStatus status;
	
    status = kar_parser_make_empty_block(token, moduleName, errors);
	if (status == KAR_PARSER_STATUS_PARSED) {
		return true;
	} else if (status == KAR_PARSER_STATUS_ERROR) {
		return false;
	}
	
	bool b = true;
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
        b = kar_parser_parse_command(token, i, moduleName, errors) && b;
	}
	return b;
}
