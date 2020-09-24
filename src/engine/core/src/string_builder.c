/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/string_builder.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define INIT_CAPACITY 16

bool k_string_builder_init(KStringBuilder* builder) {
	builder->str = (char*)malloc(sizeof(char) * INIT_CAPACITY);
	if (!builder->str) {
		return false;
	}
	builder->str[0] = 0;
	builder->length = 0;
	builder->capacity = INIT_CAPACITY;
	return true;
}

bool k_string_builder_init_book(KStringBuilder* builder, size_t book) {
	builder->str = (char*)malloc(sizeof(char) * book);
	if (!builder->str) {
		return false;
	}
	builder->str[0] = 0;
	builder->length = 0;
	builder->capacity = book;
	return true;
}

void k_string_builder_final(KStringBuilder* builder) {
	free(builder->str);
}

char* k_string_builder_final_get(KStringBuilder* builder) {
	// TODO: возможно надо применить realloc для уменьшения объема кода и обнуление значений полей структуры.
	return builder->str;
}

bool k_string_builder_push_char(KStringBuilder* str, const char c) {
	if (!str) {
		return false;
	}
	if (str->length == str->capacity - 1) {
		char* new_str = (char*)malloc(sizeof(char) * (str->capacity * 2));
		if (!new_str) {
			return false;
		}
		strcpy(new_str, str->str);
		free(str->str);
		str->str = new_str;
		str->capacity *= 2;
	}
	str->str[str->length] = c;
	str->length++;
	str->str[str->length] = 0;
	return true;
}
