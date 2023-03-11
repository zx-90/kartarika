/* Copyright © 2020,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_STREAM_H
#define KAR_STREAM_H

#include <stdbool.h>
#include <stdio.h>

#include "string.h"

typedef struct {
	FILE* data;
	bool good;
	// TODO: поле используется только для windows, для linux лишнее 
	bool eof;
} KarStream;

KarStream* kar_stream_create(const KarString* path);
void kar_stream_free(KarStream* stream);

bool kar_stream_good(KarStream* stream);
bool kar_stream_eof(KarStream* stream);
KarString kar_stream_get(KarStream* stream);

#endif // KAR_STREAM_H
