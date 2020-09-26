/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/module_error.h"

static size_t ERROR_COUNT = 0;

void kar_module_error_set(KarModule* module, KarCursor* cursor, int code, const char* description) {
	KarModuleError error = { module, *cursor, code, description };
	kar_module_error_register(&error);
}

void kar_module_error_register(KarModuleError* error) {
	fprintf(stderr, "Ошибка номер %d\n", error->code);
	fprintf(stderr, "\tМодуль: %s\n", error->module->name);
	fprintf(stderr, "\tСтрока %d, столбец %d\n", error->cursor.line, error->cursor.column);
	fprintf(stderr, "\t%s\n", error->description);
	ERROR_COUNT++;
}

size_t kar_module_error_get_count() {
	return ERROR_COUNT;
}
