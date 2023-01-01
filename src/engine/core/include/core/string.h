/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_STRING_H
#define KAR_STRING_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef const char* KarString;

#define KAR_STRING_NULL_PROTECT(str) ((str) ? (str) : "NULL")

void kar_string_list_free(char** list, size_t count);

void kar_string_list_quick_sort(char** list, size_t length);

char* kar_string_create_format(const char* format, ...);
size_t kar_string_format_args_size(const char* format, va_list args);
char* kar_string_create_format_args(const char* format, size_t size, va_list args);

char* kar_string_create_copy(const char* str);
char* kar_string_create_concat(const char* str1, const char* str2);
char *kar_string_create_replace(char* orig, char* rep, char* with);

size_t kar_string_length(const char* str);
uint32_t kar_string_get_unicode(const char* text, size_t* shift);

#endif // KAR_STRING_H
