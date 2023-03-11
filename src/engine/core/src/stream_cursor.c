/* Copyright © 2020,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/stream_cursor.h"

#include <string.h>

#include "core/alloc.h"
#include "core/string.h"
#include "core/string_builder.h"
#include "core/unicode.h"

static bool get_utf8_symbol(KarStream* stream, KarString** res) {
	KarStringBuilder builder;
	if (!kar_string_builder_init_book(&builder, 5)) {
		kar_string_builder_clear(&builder);
		return false;
	}
	
	if (kar_stream_eof(stream)) {
		*res = kar_string_builder_clear_get(&builder);
		return false;
	}
	
	KarString byte = kar_stream_get(stream);
	
	size_t count = kar_unicode_get_length_by_first_byte(byte);
	if (count == 0) {
		*res = kar_string_builder_clear_get(&builder);
		return false;
	}

	kar_string_builder_push(&builder, byte);
	for (size_t i = 1; i < count; i++) {
		byte = kar_stream_get(stream);
		kar_string_builder_push(&builder, byte);
		if (kar_unicode_is_continue_byte(byte)) {
			*res = kar_string_builder_clear_get(&builder);
			return false;
		}
	}
	
	*res = kar_string_builder_clear_get(&builder);
	return true;
}

KarStreamCursor* kar_stream_cursor_create(KarStream* stream) {
	KAR_CREATE(cursor, KarStreamCursor);
	
	cursor->stream = stream;
	kar_cursor_init(&cursor->cursor);
	cursor->currentChar = NULL;
	
	return cursor;
}

void kar_stream_cursor_free(KarStreamCursor* stream) {
	if (stream->currentChar) {
		kar_string_free(stream->currentChar);
	}
	KAR_FREE(stream);
}

bool kar_stream_cursor_is_good(const KarStreamCursor* stream) {
	return kar_stream_good(stream->stream);
}

bool kar_stream_cursor_is_equal(const KarStreamCursor* stream, const KarString* stamp) {
	return kar_string_equal(stream->currentChar, stamp);
}

bool kar_stream_cursor_is_one_of(const KarStreamCursor* stream, const KarString** stamps, size_t stamp_count) {
	return kar_string_is_one_of(stream->currentChar, stamps, stamp_count);
}

bool kar_stream_cursor_is_eof(const KarStreamCursor* stream) {
	return kar_stream_eof(stream->stream);
	
}

bool kar_stream_cursor_next(KarStreamCursor* stream) {
	if (stream->currentChar) {
		kar_string_free(stream->currentChar);
		stream->currentChar = NULL;
	}
	bool b = get_utf8_symbol(stream->stream, &stream->currentChar);
	if (!kar_string_equal(stream->currentChar, "\n")) {
		kar_cursor_next(&stream->cursor);
	} else {
		kar_cursor_next_line(&stream->cursor);
	}
	return b;
}

KarString* kar_stream_cursor_get(KarStreamCursor* stream) {
	return stream->currentChar;
}
