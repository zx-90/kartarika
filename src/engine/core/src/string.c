/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/alloc.h"

char* k_string_format(const char* format, ...) {
	va_list args;
	
	va_start(args, format);
	size_t size = (size_t)vsnprintf(NULL, 0, format, args);
	va_end(args);
	
	K_CREATES(result, char, size + 1);
	if (!result) {
		return NULL;
	}
	
	va_start(args, format);
	vsnprintf(result, size, format, args);
	va_end(args);
	
	return result;
}

size_t k_string_format_args_size(const char* format, va_list args) {
	return (size_t)vsnprintf(NULL, 0, format, args);
}

char* k_string_format_args(const char* format, size_t size, va_list args) {
	K_CREATES(result, char, size + 1);
	if (!result) {
		return NULL;
	}
	
	vsnprintf(result, size, format, args);
	
	return result;
}

char* k_string_concat(const char* str1, const char* str2)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);
	size_t len = len1 + len2;
	K_CREATES(str, char, len + 1);
	strcpy(str, str1);
	strcpy(str + len1, str2);
	str[len] = 0;
	return str;
}

void k_string_list_free(char** list, size_t count) {
	while(count--) {
		K_FREE(list[count]);
	}
	K_FREE(list);
}
