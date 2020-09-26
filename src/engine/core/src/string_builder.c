/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
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
	KAR_ALLOCS(builder->str, char, INIT_CAPACITY);
	if (!builder->str) {
		return false;
	}
	builder->str[0] = 0;
	builder->length = 0;
	builder->capacity = INIT_CAPACITY;
	return true;
}

bool kar_string_builder_init_book(KarStringBuilder* builder, size_t book) {
	KAR_ALLOCS(builder->str, char, book);
	if (!builder->str) {
		return false;
	}
	builder->str[0] = 0;
	builder->length = 0;
	builder->capacity = book;
	return true;
}

void kar_string_builder_final(KarStringBuilder* builder) {
	KAR_FREE(builder->str);
}

char* kar_string_builder_final_get(KarStringBuilder* builder) {
	// TODO: возможно надо применить realloc для уменьшения объема кода и обнуление значений полей структуры.
	return builder->str;
}

bool kar_string_builder_push_char(KarStringBuilder* str, const char c) {
	if (!str) {
		return false;
	}
	if (str->length == str->capacity - 1) {
		KAR_CREATES(new_str, char, str->capacity * 2);
		if (!new_str) {
			return false;
		}
		strcpy(new_str, str->str);
		KAR_FREE(str->str);
		str->str = new_str;
		str->capacity *= 2;
	}
	str->str[str->length] = c;
	str->length++;
	str->str[str->length] = 0;
	return true;
}
