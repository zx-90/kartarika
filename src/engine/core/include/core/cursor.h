/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef CURSOR_H
#define CURSOR_H

#include <stdbool.h>

typedef struct {
	int line;
	int column;
	bool newFile;
	bool newLine;
} KCursor;

void k_cursor_init(KCursor* cursor);
void k_cursor_next(KCursor* cursor);
void k_cursor_next_line(KCursor* cursor);

#endif // CURSOR_H
