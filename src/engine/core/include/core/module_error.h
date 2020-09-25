/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef MODULE_ERROR_H
#define MODULE_ERROR_H

#include "module.h"
#include "cursor.h"

typedef struct {
	// TODO: Module -> Module + ModuleInfo.
	KModule* module;
	KCursor cursor;
	int code;
	const char* description;
} KModuleError;

void k_module_error_set(KModule* module, KCursor* cursor, int code, const char* description);
void k_module_error_register(KModuleError* error);
size_t k_module_error_get_count();

#endif // MODULE_ERROR_H
