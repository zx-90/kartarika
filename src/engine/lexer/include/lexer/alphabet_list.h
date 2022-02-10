/* Copyright � 2022 Abdullin Timur <abdtimurrif@gmail.com>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_ALPHABET_LIST_H
#define KAR_ALPHABET_LIST_H

#include <stdint.h>
#include <stddef.h>

// ��������� ����������� �� ������ url: https://www.unicode.org/charts/
// ��������� �� ARMENIAN �� LATIN. � LATIN �� ��������� �������������� �������. ���� ��� ������� ��������� CHEROKEE.
// TODO: ��������� �� �����.

typedef enum {
	KAR_SCRIPT_UNKNOWN,

	KAR_SCRIPT_COMMON,
	KAR_SCRIPT_LATIN,
	KAR_SCRIPT_GREEK,
	KAR_SCRIPT_CYRILLIC,
	KAR_SCRIPT_ARMENIAN,
	KAR_SCRIPT_GEORGIAN,
	KAR_SCRIPT_CHEROKEE,
	KAR_SCRIPT_GLAGOLITIC,
	KAR_SCRIPT_CARIAN,
	KAR_SCRIPT_GOTHIC,
	KAR_SCRIPT_ELBASAN,
	KAR_SCRIPT_CAUCASIAN_ALBANIAN,
	KAR_SCRIPT_CYPRIOT_SYLLABARY,
	KAR_SCRIPT_CYPRO_MINOAN
} KAR_SCRIPT_NAME;

typedef struct {
	uint32_t begin;
	uint32_t end;
	KAR_SCRIPT_NAME name;
} KarScriptRange;

// ��������� ������ ���� �� ����������� � ���� �������.

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

	{0x0500, 0x052F, KAR_SCRIPT_CYRILLIC},

	{0x0531, 0x0556, KAR_SCRIPT_ARMENIAN},
	{0x0559, 0x058A, KAR_SCRIPT_ARMENIAN},
	{0x058D, 0x058F, KAR_SCRIPT_ARMENIAN},

	{0x10A0, 0x10C5, KAR_SCRIPT_GEORGIAN},
	{0x10C7, 0x10C7, KAR_SCRIPT_GEORGIAN},
	{0x10CD, 0x10CD, KAR_SCRIPT_GEORGIAN},
	{0x10D0, 0x10FF, KAR_SCRIPT_GEORGIAN},

	{0x13A0, 0x13F5, KAR_SCRIPT_CHEROKEE},
	{0x13F8, 0x13FD, KAR_SCRIPT_CHEROKEE},

	{0x1C80, 0x1C88, KAR_SCRIPT_CYRILLIC},

	{0x1C90, 0x1CBA, KAR_SCRIPT_GEORGIAN},
	{0x1CBD, 0x1CBF, KAR_SCRIPT_GEORGIAN},

	{0x1F00, 0x1F15, KAR_SCRIPT_GREEK},
	{0x1F18, 0x1F1D, KAR_SCRIPT_GREEK},
	{0x1F20, 0x1F45, KAR_SCRIPT_GREEK},
	{0x1F48, 0x1F4D, KAR_SCRIPT_GREEK},
	{0x1F50, 0x1F57, KAR_SCRIPT_GREEK},
	{0x1F59, 0x1F59, KAR_SCRIPT_GREEK},
	{0x1F5B, 0x1F5B, KAR_SCRIPT_GREEK},
	{0x1F5D, 0x1F5D, KAR_SCRIPT_GREEK},
	{0x1F5F, 0x1F7D, KAR_SCRIPT_GREEK},
	{0x1F80, 0x1FB4, KAR_SCRIPT_GREEK},
	{0x1FB6, 0x1FC4, KAR_SCRIPT_GREEK},
	{0x1FC6, 0x1FD3, KAR_SCRIPT_GREEK},
	{0x1FD6, 0x1FDB, KAR_SCRIPT_GREEK},
	{0x1FDD, 0x1FEF, KAR_SCRIPT_GREEK},
	{0x1FF2, 0x1FF4, KAR_SCRIPT_GREEK},
	{0x1FF6, 0x1FFE, KAR_SCRIPT_GREEK},

	{0x2C00, 0x2C5F, KAR_SCRIPT_GLAGOLITIC},

	{0x2D00, 0x2D25, KAR_SCRIPT_GEORGIAN},
	{0x2D27, 0x2D27, KAR_SCRIPT_GEORGIAN},
	{0x2D2D, 0x2D2D, KAR_SCRIPT_GEORGIAN},

	{0xA640, 0xA66E, KAR_SCRIPT_CYRILLIC},
	{0xA673, 0xA673, KAR_SCRIPT_CYRILLIC},
	{0xA67E, 0xA69D, KAR_SCRIPT_CYRILLIC},

	{0xAB70, 0xABBF, KAR_SCRIPT_CHEROKEE},

	{0xFB00, 0xFB06, KAR_SCRIPT_ARMENIAN},
	{0xFB13, 0xFB17, KAR_SCRIPT_ARMENIAN},
	{0xFB1D, 0xFB1D, KAR_SCRIPT_ARMENIAN},
	{0xFB1F, 0xFB36, KAR_SCRIPT_ARMENIAN},
	{0xFB38, 0xFB3C, KAR_SCRIPT_ARMENIAN},
	{0xFB3E, 0xFB3E, KAR_SCRIPT_ARMENIAN},
	{0xFB40, 0xFB41, KAR_SCRIPT_ARMENIAN},
	{0xFB43, 0xFB44, KAR_SCRIPT_ARMENIAN},
	{0xFB46, 0xFB4F, KAR_SCRIPT_ARMENIAN},

	{0x10140, 0x1018E, KAR_SCRIPT_GREEK},

	{0x102A0, 0x102D0, KAR_SCRIPT_CARIAN},

	{0x10330, 0x1034A, KAR_SCRIPT_GOTHIC},

	{0x10500, 0x10527, KAR_SCRIPT_ELBASAN},

	{0x10530, 0x10563, KAR_SCRIPT_CAUCASIAN_ALBANIAN},
	{0x1056F, 0x1056F, KAR_SCRIPT_CAUCASIAN_ALBANIAN},

	{0x10800, 0x10805, KAR_SCRIPT_CYPRIOT_SYLLABARY},
	{0x10808, 0x10808, KAR_SCRIPT_CYPRIOT_SYLLABARY},
	{0x1080A, 0x10835, KAR_SCRIPT_CYPRIOT_SYLLABARY},
	{0x10837, 0x10838, KAR_SCRIPT_CYPRIOT_SYLLABARY},
	{0x1083C, 0x1083C, KAR_SCRIPT_CYPRIOT_SYLLABARY},
	{0x1083F, 0x1083F, KAR_SCRIPT_CYPRIOT_SYLLABARY},

	{0x12F90, 0x12FF2, KAR_SCRIPT_CYPRO_MINOAN},

	{0x13A0, 0x13F5, KAR_SCRIPT_CHEROKEE},
	{0x13F8, 0x13FD, KAR_SCRIPT_CHEROKEE}
};

const size_t KAR_SCRIPT_RANGE_COUNT = sizeof(KAR_SCRIPT_RANGE_LIST) / sizeof(KarScriptRange);

#endif // KAR_ALPHABET_LIST_H