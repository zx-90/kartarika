/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/unicode.h"

size_t kar_unicode_length(const KarString* str) {
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

uint32_t kar_unicode_get(const KarString* text, size_t* shift) {
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

size_t kar_unicode_get_length_by_first_byte(KarString byte) {
	// 0xxx.xxxx
	if ((byte & 0x80) == 0) {
		return 1;
	}
	// 10xx.xxxx
	if (((byte ^ 0x80) & 0xC0) == 0) {
		return 0;
	}
	// 110x.xxxx
	if (((byte ^ 0xC0) & 0xE0) == 0) {
		return 2;
	}
	// 1110.xxxx
	if (((byte ^ 0xE0) & 0xF0) == 0) {
		return 3;
	}
	// 1111.0xxx
	if (((byte ^ 0xF0) & 0xF8) == 0) {
		return 4;
	}
	return 0;
}

bool kar_unicode_is_continue_byte(KarString byte) {
	return ((byte ^ 0x80) & 0xC0) != 0;
}
