/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/lexer.h"

#include "lexer/first_lexer.h"
#include "lexer/second_lexer.h"

bool kar_lexer_run(KarStream* stream, KarModule* module) {
	KarFirstLexer* firstLexer = kar_first_lexer_create(stream, module);
	bool b = kar_first_lexer_run(firstLexer);
	kar_first_lexer_free(firstLexer);
	if (!b) {
		return false;
	}
	return kar_second_lexer_run(module);
}
