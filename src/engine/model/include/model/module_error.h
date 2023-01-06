/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_MODULE_ERROR_H
#define KAR_MODULE_ERROR_H

#include <stddef.h>

#include "core/cursor.h"
#include "core/array.h"

typedef struct {
	KarCursor cursor;
	int code;
	char* description;
} KarModuleError;

KarModuleError* kar_module_error_create(KarCursor* cursor, int code, const char* description);

void kar_module_error_create_add(KarArray* errors, KarCursor* cursor, int code, const char* description);

void kar_module_error_free(KarModuleError* error);

void kar_module_error_print(const char* module_name, KarModuleError* error);

#endif // KAR_MODULE_ERROR_H
