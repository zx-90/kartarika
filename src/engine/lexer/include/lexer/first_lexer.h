/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef FIRST_LEXER_H
#define FIRST_LEXER_H

#include "core/module.h"
#include "core/stream_cursor.h"

// #include <string>

typedef enum {
	STATUS_UNKNOWN,
	STATUS_INDENT,

	STATUS_SPACE,
	STATUS_IDENTIFIER,
	STATUS_SIGN,
} KLexerStatus;

typedef struct {
	KStreamCursor* streamCursor;
	KLexerStatus status;
	KModule* module;
	KToken* current;
} KFirstLexer;

KFirstLexer* k_first_lexer_create(KStream* stream, KModule* module);
void k_first_lexer_free(KFirstLexer* lexer);

bool k_first_lexer_run(KFirstLexer* lexer);

#endif // FIRST_LEXER_H
