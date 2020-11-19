/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/string.h"

#include <stdio.h>
#include <string.h>

#include "core/alloc.h"

void kar_string_list_free(char** list, size_t count) {
	while(count--) {
		KAR_FREE(list[count]);
	}
	KAR_FREE(list);
}

char* kar_string_create_format(const char* format, ...) {
	va_list args;
	
	va_start(args, format);
	size_t size = (size_t)vsnprintf(NULL, 0, format, args);
	va_end(args);
	
	KAR_CREATES(result, char, size + 1);
	
	va_start(args, format);
	vsnprintf(result, size, format, args);
	va_end(args);
	
	return result;
}

size_t kar_string_format_args_size(const char* format, va_list args) {
	return (size_t)vsnprintf(NULL, 0, format, args);
}

char* kar_string_create_format_args(const char* format, size_t size, va_list args) {
	KAR_CREATES(result, char, size + 1);
	
	vsnprintf(result, size, format, args);
	result[size] = 0;
	
	return result;
}

char* kar_string_create_concat(const char* str1, const char* str2)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);
	size_t len = len1 + len2;
	KAR_CREATES(str, char, len + 1);
	strcpy(str, str1);
	strcpy(str + len1, str2);
	str[len] = 0;
	return str;
}
