/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>

typedef struct {
	size_t number;
	char* description;
} KError;

#ifdef __cplusplus
extern "C" {
#endif

KError* k_error_register(size_t number, const char* format, ...);

KError* k_error_get_last();
size_t k_error_get_last_number();
const char* k_error_get_last_description();

#ifdef __cplusplus
}
#endif

#endif // ERROR_H
