/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/stream.h"

#include <stdio.h>

#include "core/alloc.h"

// TODO: Добавить логирование сообщений об ошибках.

KarStream* kar_stream_create(const char* path) {
	FILE* file = fopen(path, "r");
	// TODO: Сообщение об ошибке (но ещё перевод нужен): printf("Error: %d (%s)\n", errno, strerror(errno))
	if (file == NULL) {
		return NULL;
	}
	KAR_CREATE(result, KarStream);
	
	result->data = file;
	result->good = true;
	
	return result;
}

void kar_stream_free(KarStream* stream) {
	if (stream == NULL) {
		return;
	}
	// TODO: Почему-то программа рушится при попытке закрыть файл. Скорее всего где-то идет работа за границей памяти.
	// TODO: проверять возвращаемое значение функции fclose.
	//fclose(stream->data);
	KAR_FREE(stream);
}

bool kar_stream_good(KarStream* stream) {
	if (stream == NULL) {
		return false;
	}
	return stream->good;
}

bool kar_stream_eof(KarStream* stream) {
	if (stream == NULL) {
		return false;
	}
	return feof(stream->data);
}

char kar_stream_get(KarStream* stream) {
	if (stream == NULL) {
		return 0;
	}
	int result = fgetc(stream->data);
	if (result == EOF) {
		return 0;
	}
	return (char)result;
}
