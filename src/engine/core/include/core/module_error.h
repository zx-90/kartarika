/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_MODULE_ERROR_H
#define KAR_MODULE_ERROR_H

#include "module.h"
#include "cursor.h"

typedef struct {
	// TODO: Module -> Module + ModuleInfo.
	KarModule* module;
	KarCursor cursor;
	int code;
	const char* description;
} KarModuleError;

void kar_module_error_set(KarModule* module, KarCursor* cursor, int code, const char* description);
void kar_module_error_register(KarModuleError* error);
size_t kar_module_error_get_count();

#endif // KAR_MODULE_ERROR_H
