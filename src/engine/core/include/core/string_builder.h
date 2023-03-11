/* Copyright © 2020,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_STRING_BUILDER_H
#define KAR_STRING_BUILDER_H

#include <stddef.h>
#include <stdbool.h>

#include "string.h"

typedef struct {
	KarString* str;
	size_t length;
	size_t capacity;
} KarStringBuilder;

bool kar_string_builder_init(KarStringBuilder* builder);
bool kar_string_builder_init_book(KarStringBuilder* builder, size_t book);
void kar_string_builder_clear(KarStringBuilder* builder);
KarString* kar_string_builder_clear_get(KarStringBuilder* builder);

bool kar_string_builder_push(KarStringBuilder* str, const KarString c);

#endif // KAR_STRING_BUILDER_H
