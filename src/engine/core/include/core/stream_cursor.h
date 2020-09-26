/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_STREAM_CURSOR_H
#define KAR_STREAM_CURSOR_H

#include "stream.h"
#include "cursor.h"

typedef struct {
	KarStream* stream;
	KarCursor cursor;
	char* currentChar;
} KarStreamCursor;

KarStreamCursor* kar_stream_cursor_create(KarStream* stream);

void kar_stream_cursor_free(KarStreamCursor* stream);

bool kar_stream_cursor_is_good(const KarStreamCursor* stream);
bool kar_stream_cursor_is_equal(const KarStreamCursor* stream, const char* stamp);
bool kar_stream_cursor_is_one_of(const KarStreamCursor* stream, const char** stamps, size_t stamp_count);
bool kar_stream_cursor_is_eof(const KarStreamCursor* stream);

bool kar_stream_cursor_next(KarStreamCursor* stream);
char* kar_stream_cursor_get(KarStreamCursor* stream);

#endif // KAR_STREAM_CURSOR_H
