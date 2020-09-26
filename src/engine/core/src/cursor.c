/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/cursor.h"

void kar_cursor_init(KarCursor* cursor) {
	cursor->line = 1;
	cursor->column = 1;
	cursor->newFile = true;
	cursor->newLine = true;
}

void kar_cursor_next(KarCursor* cursor) {
	if (cursor->newFile) {
		cursor->newFile = false;
		cursor->newLine = false;
		return;
	}
	if (cursor->newLine) {
		cursor->newLine = false;
		return;
	}
	cursor->column++;
}

void kar_cursor_next_line(KarCursor* cursor) {
	cursor->line++;
	cursor->column = 1;
	cursor->newLine = true;
}
