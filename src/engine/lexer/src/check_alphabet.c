/* Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/check_alphabet.h"

#include <string.h>
#include <stdio.h>

#include "core/string.h"
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

bool kar_check_identifiers_alphabet(const char* text) {
	uint32_t buffer = 0;
	size_t len = strlen(text);
	KAR_SCRIPT_NAME currentName = KAR_SCRIPT_UNKNOWN;
	KAR_SCRIPT_NAME name = KAR_SCRIPT_UNKNOWN;
	for (size_t count = 0; count < len;) {
		buffer = kar_string_get_unicode(&text[count], &count);
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