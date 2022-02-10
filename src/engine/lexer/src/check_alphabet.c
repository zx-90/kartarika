/* Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/check_alphabet.h"

#include "lexer/alphabet_list.h"

static KAR_SCRIPT_NAME get_script_name(uint32_t buffer) {
	int low, high, middle;
	low = 0;
	high = KAR_SCRIPT_RANGE_COUNT - 1;
	while (low <= high)
	{
		middle = (low + high) / 2;
		KarScriptRange* range = &KAR_SCRIPT_RANGE_LIST[middle];
		if (buffer < range->begin)
			high = middle - 1;
		else if (buffer > range->end)
			low = middle + 1;
		else if (range->begin <= buffer && range->end >= buffer) {
			return range->name;
		}
	}
	return KAR_SCRIPT_UNKNOWN;
}

static uint32_t get_unicode(const char* text, int* shift) {
	if (!(text[0] & 0b10000000)) {
		*shift += 1;
		return text[0] & 0b01111111;
	}
	if ((text[0] & 0b11000000) && !(text[0] & 0b00100000) && (text[1] & 0b10000000) && !(text[1] & 0b01000000)) {
		*shift += 2;
		return(text[0] & 0b00011111) * 0x40 + (text[1] & 0b00111111);
	}
	if ((text[0] & 0b11100000) && !(text[0] & 0b00010000) && (text[1] & 0b10000000) && !(text[1] & 0b01000000) && (text[2] & 0b10000000) && !(text[2] & 0b01000000)) {
		*shift += 3;
		return(text[0] & 0b00001111) * 0x40 * 0x40 + (text[1] & 0b00111111) * 0x40 + (text[2] & 0b00111111);
	}
	if ((text[0] & 0b11110000) && !(text[0] & 0b00001000) && (text[1] & 0b10000000) && !(text[1] & 0b01000000) && (text[2] & 0b10000000) && !(text[2] & 0b01000000) && (text[3] & 0b10000000) && !(text[3] & 0b01000000)) {
		*shift += 4;
		return(text[0] & 0b00001111) * 0x40 * 0x40 * 0x40 + (text[1] & 0b00111111) * 0x40 * 0x40 + (text[2] & 0b00111111) * 0x40 + (text[3] & 0b00111111);
	}
	return 0;
}

bool kar_check_identifiers_alphabet(const char* text) {
	uint32_t buffer = 0;
	KAR_SCRIPT_NAME currentName = KAR_SCRIPT_UNKNOWN;
	KAR_SCRIPT_NAME name = KAR_SCRIPT_UNKNOWN;
	for (int count = 0; count < strlen(text);) {
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