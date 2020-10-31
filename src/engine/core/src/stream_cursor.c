/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/stream_cursor.h"

#include <string.h>

#include "core/alloc.h"
#include "core/string_builder.h"

static bool get_utf8_symbol(KarStream* stream, char** res) {
	KarStringBuilder builder;
	if (!kar_string_builder_init_book(&builder, 5)) {
		kar_string_builder_clear(&builder);
		return false;
	}
	
	if (kar_stream_eof(stream)) {
		*res = kar_string_builder_clear_get(&builder);
		return false;
	}
	
	char byte = kar_stream_get(stream);
	
	if ((byte & 0x80) == 0) {
		kar_string_builder_push_char(&builder, byte);
		*res = kar_string_builder_clear_get(&builder);
		return true;
	}
	
	if (((byte ^ 0x80) & 0xC0) == 0) {
		kar_string_builder_push_char(&builder, byte);
		*res = kar_string_builder_clear_get(&builder);
		return false;
	}

	int count = 0;
	if (((byte ^ 0xC0) & 0xE0) == 0) {
		count = 2;
	} else if (((byte ^ 0xE0) & 0xF0) == 0) {
		count = 3;
	} else if (((byte ^ 0xF0) & 0xF8) == 0) {
		count = 4;
	} else {
		*res = kar_string_builder_clear_get(&builder);
		return false;
	}

	kar_string_builder_push_char(&builder, byte);
	for (int i = 1; i < count; i++) {
		byte = kar_stream_get(stream);
		kar_string_builder_push_char(&builder, byte);
		if (((byte ^ 0x80) & 0xC0) != 0) {
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
		KAR_FREE(stream->currentChar);
	}
	KAR_FREE(stream);
}

bool kar_stream_cursor_is_good(const KarStreamCursor* stream) {
	return kar_stream_good(stream->stream);
}

bool kar_stream_cursor_is_equal(const KarStreamCursor* stream, const char* stamp) {
	return !strcmp(stream->currentChar, stamp);
}

bool kar_stream_cursor_is_one_of(const KarStreamCursor* stream, const char** stamps, size_t stamp_count) {
	while (stamp_count) {
		if (!strcmp(stream->currentChar, *stamps)) {
			return true;
		}
		stamp_count--;
		stamps++;
	}
	return false;
}

bool kar_stream_cursor_is_eof(const KarStreamCursor* stream) {
	return kar_stream_eof(stream->stream);
	
}

bool kar_stream_cursor_next(KarStreamCursor* stream) {
	if (stream->currentChar) {
		KAR_FREE(stream->currentChar);
		stream->currentChar = NULL;
	}
	bool b = get_utf8_symbol(stream->stream, &stream->currentChar);
	if (strcmp(stream->currentChar, "\n")) {
		kar_cursor_next(&stream->cursor);
	} else {
		kar_cursor_next_line(&stream->cursor);
	}
	return b;
}

char* kar_stream_cursor_get(KarStreamCursor* stream) {
	return stream->currentChar;
}
