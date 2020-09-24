/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/module_error.h"

static size_t ERROR_COUNT = 0;

void k_module_error_set(KModule* module, KCursor* cursor, int code, const char* description) {
	KModuleError error = { module, *cursor, code, description };
	k_module_error_register(&error);
}

void k_module_error_register(KModuleError* error) {
	fprintf(stderr, "Ошибка номер %d\n", error->code);
	fprintf(stderr, "\tМодуль: %s\n", error->module->name);
	fprintf(stderr, "\tСтрока %d, столбец %d\n", error->cursor.line, error->cursor.column);
	fprintf(stderr, "\t%s\n", error->description);
	ERROR_COUNT++;
}

size_t k_module_error_get_count() {
	return ERROR_COUNT;
}
