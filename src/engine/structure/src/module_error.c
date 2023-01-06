/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "structure/module_error.h"

#include "core/alloc.h"
#include "core/string.h"

KarModuleError* kar_module_error_create(KarCursor* cursor, int code, const char* description) {
	KAR_CREATE(error, KarModuleError);
	
	error->cursor = *cursor;
	error->code = code;
	error->description = kar_string_create_copy(description);
	
	return error;
}

void kar_module_error_create_add(KarArray* errors, KarCursor* cursor, int code, const char* description) {
	KarModuleError* error = kar_module_error_create(cursor, code, description);
	kar_array_add(errors, (void*)error);
}

void kar_module_error_free(KarModuleError* error) {
	KAR_FREE(error->description);
	KAR_FREE(error);
}

void kar_module_error_print(const char* module_name, KarModuleError* error) {
	fprintf(stdout, "Ошибка номер %d\n", error->code);
	fprintf(stdout, "\tМодуль: %s\n", module_name);
	fprintf(stdout, "\tСтрока %d, столбец %d\n", error->cursor.line, error->cursor.column);
	fprintf(stdout, "\t%s\n", error->description);
	fflush(stdout);
}
