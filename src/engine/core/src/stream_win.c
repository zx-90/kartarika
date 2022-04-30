/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021,2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/stream.h"

#include <stdio.h>

#include <windows.h>

#include "core/alloc.h"

// TODO: Добавить логирование сообщений об ошибках.

KarStream* kar_stream_create(const char* path) {
	FILE* file = kar_file_system_create_handle(path);
	// TODO: Сообщение об ошибке (но ещё перевод нужен): printf("Error: %d (%s)\n", errno, strerror(errno))
	if (file == NULL) {
		return NULL;
	}
	KAR_CREATE(result, KarStream);
	
	result->data = file;
	result->good = true;
	result->eof = false;
	
	return result;
}

void kar_stream_free(KarStream* stream) {
	if (stream == NULL) {
		return;
	}
	// TODO: Почему-то программа рушится при попытке закрыть файл. Скорее всего где-то идет работа за границей памяти.
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
	return stream->eof;
}

char kar_stream_get(KarStream* stream) {
	if (stream == NULL) {
		return 0;
	}
	DWORD read;
	char result;
	if (!ReadFile(stream->data, &result, 1, &read, NULL)) {
		stream->good = false;
		return 0;
	}
	if (read == 0) {
		stream->eof = true;
		return 0;
	}
	return result;
}
