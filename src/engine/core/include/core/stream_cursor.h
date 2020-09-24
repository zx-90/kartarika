/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef STREAM_CURSOR_H
#define STREAM_CURSOR_H

#include "stream.h"
#include "cursor.h"

typedef struct {
	KStream* stream;
	KCursor cursor;
	char* currentChar;
} KStreamCursor;

#ifdef __cplusplus
extern "C" {
#endif

KStreamCursor* k_stream_cursor_create(KStream* stream);

void k_stream_cursor_free(KStreamCursor* stream);

bool k_stream_cursor_is_good(const KStreamCursor* stream);
bool k_stream_cursor_is_equal(const KStreamCursor* stream, const char* stamp);
bool k_stream_cursor_is_one_of(const KStreamCursor* stream, const char** stamps, size_t stamp_count);
bool k_stream_cursor_is_eof(const KStreamCursor* stream);

bool k_stream_cursor_next(KStreamCursor* stream);
char* k_stream_cursor_get(KStreamCursor* stream);

#ifdef __cplusplus
}
#endif

#endif // STREAM_CURSOR_H
