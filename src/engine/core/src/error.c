/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/error.h"

#include <stddef.h>
#include <stdlib.h>

#include "core/alloc.h"
#include "core/string.h"

KError error = {0, NULL};

KError* k_error_register(size_t number, const char* format, ...) {
	error.number = number;
	if (error.description) {
		K_FREE(error.description);
	}
	
	va_list args;
	
	va_start(args, format);
	size_t size = k_string_format_args_size(format, args);
	va_end(args);
	
	va_start(args, format);
	error.description = k_string_format_args(format, size, args);
	va_end(args);
	
	return &error;
}

KError* k_error_get_last() {
	return &error;
}

size_t k_error_get_last_number() {
	return error.number;
}

const char* k_error_get_last_description() {
	return error.description;
}
