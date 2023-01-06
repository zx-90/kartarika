/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_FIRST_LEXER_H
#define KAR_FIRST_LEXER_H

#include "core/stream_cursor.h"
#include "structure/module.h"

typedef enum {
	KAR_LEXER_STATUS_UNKNOWN,
	KAR_LEXER_STATUS_INDENT,

	KAR_LEXER_STATUS_SPACE,
	KAR_LEXER_STATUS_IDENTIFIER,
	KAR_LEXER_STATUS_SIGN,
} KarLexerStatus;

typedef struct {
	KarStreamCursor* streamCursor;
	KarLexerStatus status;
	KarModule* module;
	KarToken* current;
} KarFirstLexer;

KarFirstLexer* kar_first_lexer_create(KarStream* stream, KarModule* module);
void kar_first_lexer_free(KarFirstLexer* lexer);

bool kar_first_lexer_run(KarFirstLexer* lexer);

#endif // KAR_FIRST_LEXER_H
