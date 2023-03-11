/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_UNICODE_H
#define KAR_UNICODE_H

#include "string.h"

size_t kar_unicode_length(const KarString* str);
uint32_t kar_unicode_get(const KarString* text, size_t* shift);
size_t kar_unicode_get_length_by_first_byte(KarString byte);
bool kar_unicode_is_continue_byte(KarString byte);

#endif // KAR_UNICODE_H
