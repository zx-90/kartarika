/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
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

static size_t partition(char** list, size_t length) {
	size_t p = length - 1;
	size_t firsthigh = 0;
	
	for (size_t i = 0; i < length; ++i) {
		if (strcmp(list[i], list[p]) < 0) {
			char* temp = list[i];
			list[i] = list[firsthigh];
			list[firsthigh] = temp;
			firsthigh++;
		}
	}
	
	char* temp = list[p];
	list[p] = list[firsthigh];
	list[firsthigh] = temp;
	
	return firsthigh;
}

void kar_string_list_quick_sort(char** list, size_t length) {
	if (length <= 1) {
		return;
	}
	size_t p = partition(list, length);
	kar_string_list_quick_sort(list, p);
	kar_string_list_quick_sort(&(list[p + 1]), length - p - 1);
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

char* kar_string_create_copy(const char* str) {
	size_t len = strlen(str);
	KAR_CREATES(result, char, len + 1);
	strcpy(result, str);
	result[len] = 0;
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

char *kar_string_create_replace(char* orig, char* rep, char* with) {
    //char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    size_t len_rep;  // length of rep (the string to remove)
    size_t len_with; // length of with (the string to replace rep with)
    size_t len_front; // distance between rep and end of last rep
    size_t count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }
    
    KAR_CREATES(result, char, strlen(orig) + (len_with - len_rep) * count + 1);
    tmp = result;

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = (size_t)(ins - orig);
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

size_t kar_string_length(const char* str) {
	size_t res = 0;
	while (*str) {
		if ((*str & 0x80) == 0 ||
			((*str ^ 0xC0) & 0xE0) == 0 ||
			((*str ^ 0xE0) & 0xF0) == 0 ||
			((*str ^ 0xF0) & 0xF8) == 0
		) {
			res++;
		}
		str++;
	}
	return res;
}

uint32_t kar_string_get_unicode(const char* text, size_t* shift) {
	if (!(text[0] & 0x80)) {
		*shift += 1;
		return text[0] & 0x7F;
	}
	if ((text[0] & 0xC0) && !(text[0] & 0x20) && (text[1] & 0x80) && !(text[1] & 0x40)) {
		*shift += 2;
		return (uint32_t)(text[0] & 0x1F) * 0x40 + (text[1] & 0x3F);
	}
	if ((text[0] & 0xE0) && !(text[0] & 0x10) && (text[1] & 0x80) && !(text[1] & 0x40) && (text[2] & 0x80) && !(text[2] & 0x40)) {
		*shift += 3;
		return (uint32_t)(text[0] & 0x0F) * 0x40 * 0x40 + (uint32_t)(text[1] & 0x3F) * 0x40 + (text[2] & 0x3F);
	}
	if ((text[0] & 0xF0) && !(text[0] & 0x08) && (text[1] & 0x80) && !(text[1] & 0x40) && (text[2] & 0x80) && !(text[2] & 0x40) && (text[3] & 0x80) && !(text[3] & 0x40)) {
		*shift += 4;
		return (uint32_t)(text[0] & 0x0F) * 0x40 * 0x40 * 0x40 + (uint32_t)(text[1] & 0x3F) * 0x40 * 0x40 + (uint32_t)(text[2] & 0x3F) * 0x40 + (text[3] & 0x3F);
	}
	return 0;
}

