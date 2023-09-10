/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/project_error.h"

#include "core/alloc.h"
#include "core/string.h"

KarProjectError* kar_project_error_create(KarString *moduleName, KarCursor* cursor, int code, const KarString* description) {
	KAR_CREATE(error, KarProjectError);
	
    error->moduleName = kar_string_create(moduleName);
	error->cursor = *cursor;
	error->code = code;
	error->description = kar_string_create(description);
	
	return error;
}

void kar_project_error_free(KarProjectError* error) {
	kar_string_free(error->description);
	KAR_FREE(error);
}

#define KAR_PROJECT_ERROR_PRINT_STAMP ("Ошибка номер %d\n" "\tМодуль: %s\n" "\tСтрока %d, столбец %d\n" "\t%s\n")

void kar_project_error_print(KarProjectError* error) {
	fprintf(stdout, KAR_PROJECT_ERROR_PRINT_STAMP,
		error->code,
		error->moduleName,
		error->cursor.line, error->cursor.column,
		error->description
	);
	fflush(stdout);
}

KarString* kar_project_error_create_string(KarProjectError* error) {
	return kar_string_create_format(KAR_PROJECT_ERROR_PRINT_STAMP,
		error->code,
		error->moduleName,
		error->cursor.line, error->cursor.column,
		error->description
	);
}
