/* Copyright © 2020,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_ERROR_H
#define KAR_ERROR_H

#include <stddef.h>

#include "core/string.h"

typedef struct {
	size_t number;
	KarString* description;
} KarError;

KarError* kar_error_register(size_t number, const KarString* format, ...);

KarError* kar_error_get_last();
size_t kar_error_get_last_number();
const KarString* kar_error_get_last_description();

#endif // KAR_ERROR_H
