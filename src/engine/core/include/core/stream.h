/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>
#include <stdio.h>

typedef struct {
	FILE* data;
	bool good;
} KStream;

KStream* k_stream_create(const char* path);

bool k_stream_good(KStream* stream);
bool k_stream_eof(KStream* stream);
char k_stream_get(KStream* stream);

void k_stream_free(KStream* stream);

#endif // STREAM_H
