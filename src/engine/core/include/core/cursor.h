/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_CURSOR_H
#define KAR_CURSOR_H

#include <stdbool.h>

typedef struct {
	int line;
	int column;
	bool newFile;
	bool newLine;
} KarCursor;

void kar_cursor_init(KarCursor* cursor);
void kar_cursor_next(KarCursor* cursor);
void kar_cursor_next_line(KarCursor* cursor);

#endif // KAR_CURSOR_H
