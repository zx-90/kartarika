/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_STRING_H
#define KAR_STRING_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// TODO: Применить везде.
typedef char KarString;

KarString* kar_string_create(const char* str);
void kar_string_free(KarString* str);

KarString* kar_string_create_format(const char* format, ...);
size_t kar_string_format_args_size(const char* format, va_list args);
KarString* kar_string_create_format_args(const char* format, size_t size, va_list args);

KarString* kar_string_create_concat(const KarString* str1, const KarString* str2);
KarString* kar_string_create_replace(KarString* orig, const KarString* rep, const KarString* with);

bool kar_string_less(const KarString* str1, const KarString* str2);
bool kar_string_equal(const KarString* str1, const KarString* str2);

bool kar_string_is_one_of(const KarString* str, const KarString** stamps, size_t stamp_count);
KarString* kar_string_encode_hex(const KarString* input);

#endif // KAR_STRING_H
