/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/string.h"

#include <stdio.h>
#include <string.h>

#include "core/alloc.h"

KarString* kar_string_create(const char* str) {
	size_t len = strlen(str);
	KAR_CREATES(result, char, len + 1);
	strcpy(result, str);
	result[len] = 0;
	return result;
}

void kar_string_free(KarString* str) {
	KAR_FREE(str);
}

KarString* kar_string_create_format(const char* format, ...) {
	va_list args;
	
	va_start(args, format);
	size_t size = (size_t)vsnprintf(NULL, 0, format, args);
	va_end(args);
	
	KAR_CREATES(result, KarString, size + 1);
	
	va_start(args, format);
	vsnprintf(result, size + 1, format, args);
	va_end(args);
	
	return result;
}

size_t kar_string_format_args_size(const char* format, va_list args) {
	return (size_t)vsnprintf(NULL, 0, format, args);
}

KarString* kar_string_create_format_args(const char* format, size_t size, va_list args) {
	KAR_CREATES(result, KarString, size + 1);
	
	vsnprintf(result, size, format, args);
	result[size] = 0;
	
	return result;
}

KarString* kar_string_create_concat(const KarString* str1, const KarString* str2)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);
	size_t len = len1 + len2;
	KAR_CREATES(str, KarString, len + 1);
	strcpy(str, str1);
	strcpy(str + len1, str2);
	str[len] = 0;
	return str;
}

KarString *kar_string_create_replace(KarString* orig, const KarString* rep, const KarString* with) {
    //KarString *result; // the return string
    KarString *ins;    // the next insert point
    KarString *tmp;    // varies
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

bool kar_string_less(const KarString* str1, const KarString* str2) {
	return strcmp(str1, str2) < 0;
}

bool kar_string_equal(const KarString* str1, const KarString* str2) {
	return strcmp(str1, str2) == 0;
}

bool kar_string_is_one_of(const KarString* str, const KarString** stamps, size_t stamp_count) {
	while (stamp_count) {
		if (kar_string_equal(str, *stamps)) {
			return true;
		}
		stamp_count--;
		stamps++;
	}
	return false;
}

KarString* kar_string_encode_hex(const KarString* input) {
    static const KarString* const lut = "0123456789ABCDEF";
    size_t len = strlen(input);

	KAR_CREATES(output, KarString, len * 2 + 1);
	KarString* cur = output;
	while(!*input) {
		*cur++ = lut[*input >> 4];
		*cur++ = lut[*input & 15];
		input++;
	}
	*cur = 0;
    return output;
}
