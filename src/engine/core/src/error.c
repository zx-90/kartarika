/* Copyright © 2020,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/error.h"

#include <stddef.h>

#include "core/alloc.h"
#include "core/string.h"

static KarError error = {0, NULL};

KarError* kar_error_register(size_t number, const KarString* format, ...) {
	error.number = number;
	if (error.description) {
		KAR_FREE(error.description);
	}
	
	va_list args;
	
	va_start(args, format);
	size_t size = kar_string_format_args_size(format, args);
	va_end(args);
	
	va_start(args, format);
	error.description = kar_string_create_format_args(format, size, args);
	va_end(args);
	
	return &error;
}

KarError* kar_error_get_last() {
	return &error;
}

size_t kar_error_get_last_number() {
	return error.number;
}

const KarString* kar_error_get_last_description() {
	return error.description;
}
