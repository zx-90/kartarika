/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
	char* str;
	size_t length;
	size_t capacity;
} KStringBuilder;

bool k_string_builder_init(KStringBuilder* builder);
bool k_string_builder_init_book(KStringBuilder* builder, size_t book);
void k_string_builder_final(KStringBuilder* builder);
char* k_string_builder_final_get(KStringBuilder* builder);

bool k_string_builder_push_char(KStringBuilder* str, const char c);

#endif // STRING_BUILDER_H
