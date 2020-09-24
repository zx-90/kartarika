/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/stream_cursor.h"

#include <stdlib.h>
#include <string.h>

#include "core/string_builder.h"

static bool get_utf8_symbol(KStream* stream, char** res) {
	KStringBuilder builder;
	if (!k_string_builder_init_book(&builder, 5)) {
		return false;
	}
	
	if (k_stream_eof(stream)) {
		return false;
	}
	
	char byte = k_stream_get(stream);
	
	if ((byte & 0x80) == 0) {
		k_string_builder_push_char(&builder, byte);
		*res = k_string_builder_final_get(&builder);
		return true;
	}
	
	if (((byte ^ 0x80) & 0xC0) == 0) {
		k_string_builder_push_char(&builder, byte);
		*res = k_string_builder_final_get(&builder);
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
		*res = k_string_builder_final_get(&builder);
		return false;
	}

	k_string_builder_push_char(&builder, byte);
	for (int i = 1; i < count; i++) {
		byte = k_stream_get(stream);
		k_string_builder_push_char(&builder, byte);
		if (((byte ^ 0x80) & 0xC0) != 0) {
			*res = k_string_builder_final_get(&builder);
			return false;
		}
	}
	
	*res = k_string_builder_final_get(&builder);
	return true;
}

KStreamCursor* k_stream_cursor_create(KStream* stream) {
	KStreamCursor* cursor = (KStreamCursor*)malloc(sizeof(KStreamCursor));
	if (!cursor) {
		return NULL;
	}
	
	cursor->stream = stream;
	k_cursor_init(&cursor->cursor);
	cursor->currentChar = NULL;
	
	return cursor;
}

void k_stream_cursor_free(KStreamCursor* stream) {
	if (stream->currentChar) {
		free(stream->currentChar);
	}
	free(stream);
}

bool k_stream_cursor_is_good(const KStreamCursor* stream) {
	return k_stream_good(stream->stream);
}

bool k_stream_cursor_is_equal(const KStreamCursor* stream, const char* stamp) {
	return !strcmp(stream->currentChar, stamp);
}

bool k_stream_cursor_is_one_of(const KStreamCursor* stream, const char** stamps, size_t stamp_count) {
	while (stamp_count) {
		if (!strcmp(stream->currentChar, *stamps)) {
			return true;
		}
		stamp_count--;
		stamps++;
	}
	return false;
}

bool k_stream_cursor_is_eof(const KStreamCursor* stream) {
	return k_stream_eof(stream->stream);
	
}

bool k_stream_cursor_next(KStreamCursor* stream) {
	if (stream->currentChar) {
		free(stream->currentChar);
		stream->currentChar = NULL;
	}
	bool b = get_utf8_symbol(stream->stream, &stream->currentChar);
	if (strcmp(stream->currentChar, "\n")) {
		k_cursor_next(&stream->cursor);
	} else {
		k_cursor_next_line(&stream->cursor);
	}
	return b;
}

char* k_stream_cursor_get(KStreamCursor* stream) {
	return stream->currentChar;
}
