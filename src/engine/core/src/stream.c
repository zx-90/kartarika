/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/stream.h"

#include <stdio.h>
#include <stdlib.h>

// TODO: Добавить логирование сообщений об ошибках.

KStream* k_stream_create(const char* path) {
	FILE* file = fopen(path, "r");
	// TODO: Сообщение об ошибке (но ещё перевод нужен): printf("Error: %d (%s)\n", errno, strerror(errno))
	if (file == NULL) {
		return NULL;
	}
	
	KStream* result = (KStream*)malloc(sizeof(KStream));
	if (result == NULL) {
		return NULL;
	}
	
	result->data = file;
	result->good = true;
	
	return result;
}

bool k_stream_good(KStream* stream) {
	if (stream == NULL) {
		return false;
	}
	return stream->good;
}

bool k_stream_eof(KStream* stream) {
	if (stream == NULL) {
		return false;
	}
	return feof(stream->data);
}

char k_stream_get(KStream* stream) {
	if (stream == NULL) {
		return 0;
	}
	int result = fgetc(stream->data);
	if (result == EOF) {
		return 0;
	}
	return (char)result;
}

void k_stream_free(KStream* stream) {
	if (stream == NULL) {
		return;
	}
	// TODO: проверять возвращаемое значение функции fclose.
	fclose(stream->data);
	free(stream);
}
