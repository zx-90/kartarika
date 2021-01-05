/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/module_error.h"

#include "core/alloc.h"
#include "core/string.h"

KarModuleError* kar_module_error_create(KarCursor* cursor, int code, const char* description) {
	KAR_CREATE(error, KarModuleError);
	
	error->cursor = *cursor;
	error->code = code;
	error->description = kar_string_create_copy(description);
	
	return error;
}

void kar_module_error_free(KarModuleError* error) {
	KAR_FREE(error->description);
	KAR_FREE(error);
}

void kar_module_error_print(const char* module_name, KarModuleError* error) {
	fprintf(stderr, "Ошибка номер %d\n", error->code);
	fprintf(stderr, "\tМодуль: %s\n", module_name);
	fprintf(stderr, "\tСтрока %d, столбец %d\n", error->cursor.line, error->cursor.column);
	fprintf(stderr, "\t%s\n", error->description);
}
