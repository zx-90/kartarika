/* Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/check_alphabet.h"

#include <string.h>
#include <stdio.h>

#include "lexer/alphabet_list.h"

static KAR_SCRIPT_NAME get_script_name(uint32_t buffer) {
	size_t low, high, middle;
	low = 0;
	high = KAR_SCRIPT_RANGE_COUNT - 1;
	while (low <= high)
	{
		middle = (low + high) / 2;
		const KarScriptRange* range = &KAR_SCRIPT_RANGE_LIST[middle];
		if (buffer < range->begin) {
			if (middle == 0) {
				return KAR_SCRIPT_UNKNOWN;
			}
			high = middle - 1;
		} else if (buffer > range->end) {
			low = middle + 1;
		} else if (range->begin <= buffer && range->end >= buffer) {
			return range->name;
		}
	}
	return KAR_SCRIPT_UNKNOWN;
}

static uint32_t get_unicode(const char* text, size_t* shift) {
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

bool kar_check_identifiers_alphabet(const char* text) {
	uint32_t buffer = 0;
	KAR_SCRIPT_NAME currentName = KAR_SCRIPT_UNKNOWN;
	KAR_SCRIPT_NAME name = KAR_SCRIPT_UNKNOWN;
	for (size_t count = 0; count < strlen(text);) {
		buffer = get_unicode(&text[count], &count);
		currentName = get_script_name(buffer);
		if (currentName == KAR_SCRIPT_UNKNOWN) {
			return false;
		}
		if (currentName == KAR_SCRIPT_COMMON) {
			continue;
		}
		if (name == KAR_SCRIPT_UNKNOWN) {
			name = currentName;
			continue;
		}
		if (name != currentName) {
			return false;
		}
	}
	return true;
}