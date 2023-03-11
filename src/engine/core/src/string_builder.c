/* Copyright © 2020,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/string_builder.h"

#include <stddef.h>
#include <string.h>

#include "core/alloc.h"

#define INIT_CAPACITY (size_t)16

bool kar_string_builder_init(KarStringBuilder* builder) {
	return kar_string_builder_init_book(builder, INIT_CAPACITY);
}

bool kar_string_builder_init_book(KarStringBuilder* builder, size_t book) {
	KAR_ALLOCS(builder->str, KarString, book);
	builder->str[0] = 0;
	builder->length = 0;
	builder->capacity = book;
	return true;
}

void kar_string_builder_clear(KarStringBuilder* builder) {
	kar_string_free(builder->str);
}

KarString* kar_string_builder_clear_get(KarStringBuilder* builder) {
	KAR_REALLOC(builder->str, KarString, builder->length + 1);
	return builder->str;
}

bool kar_string_builder_push(KarStringBuilder* str, const KarString c) {
	if (str->length == str->capacity - 1) {
		str->capacity *= 2;
		KAR_REALLOC(str->str, KarString, str->capacity);
	}
	str->str[str->length] = c;
	str->length++;
	str->str[str->length] = 0;
	return true;
}
