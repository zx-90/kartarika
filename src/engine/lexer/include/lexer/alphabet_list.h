/* Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_ALPHABET_LIST_H
#define KAR_ALPHABET_LIST_H

#include <stdint.h>

typedef enum {
	KAR_SCRIPT_UNKNOWN,

	KAR_SCRIPT_COMMON,
	KAR_SCRIPT_LATIN,
	KAR_SCRIPT_GREEK,
	KAR_SCRIPT_CYRILLIC,
	KAR_SCRIPT_CHEROKEE
} KAR_SCRIPT_NAME;

typedef struct {
	uint32_t begin;
	uint32_t end;
	KAR_SCRIPT_NAME name;
} KarScriptRange;

const KarScriptRange KAR_SCRIPT_RANGE_LIST[] = {
	{0x0030, 0x0039, KAR_SCRIPT_COMMON},
	{0x0041, 0x005A, KAR_SCRIPT_LATIN},
	{0x005F, 0x005F, KAR_SCRIPT_COMMON},
	{0x0061, 0x007A, KAR_SCRIPT_LATIN},

	{0x0370, 0x0377, KAR_SCRIPT_GREEK},
	{0x037A, 0x037F, KAR_SCRIPT_GREEK},
	{0x0384, 0x038A, KAR_SCRIPT_GREEK},
	{0x038C, 0x038C, KAR_SCRIPT_GREEK},
	{0x038E, 0x03A1, KAR_SCRIPT_GREEK},
	{0x03A3, 0x03FF, KAR_SCRIPT_GREEK},

	{0x0400, 0x0482, KAR_SCRIPT_CYRILLIC},
	{0x048A, 0x04FF, KAR_SCRIPT_CYRILLIC},

	{0x13A0, 0x13F5, KAR_SCRIPT_CHEROKEE},
	{0x13F8, 0x13FD, KAR_SCRIPT_CHEROKEE}
};

const size_t KAR_SCRIPT_RANGE_COUNT = sizeof(KAR_SCRIPT_RANGE_LIST) / sizeof(KarScriptRange);

#endif // KAR_ALPHABET_LIST_H