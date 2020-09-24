/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef STRING_H
#define STRING_H

#include <stdarg.h>
#include <stddef.h>

typedef const char* KString;

void k_string_list_free(char** list, size_t count);

char* k_string_format(const char* format, ...);

size_t k_string_format_args_size(const char* format, va_list args);
char* k_string_format_args(const char* format, size_t size, va_list args);
char* k_string_concat(const char* str1, const char* str2);

#endif // STRING_H
