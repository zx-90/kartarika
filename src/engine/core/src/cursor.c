/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/cursor.h"

void k_cursor_init(KCursor* cursor) {
	cursor->line = 1;
	cursor->column = 1;
	cursor->newFile = true;
	cursor->newLine = true;
}

void k_cursor_next(KCursor* cursor) {
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

void k_cursor_next_line(KCursor* cursor) {
	cursor->line++;
	cursor->column = 1;
	cursor->newLine = true;
}
