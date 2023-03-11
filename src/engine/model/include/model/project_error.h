/* Copyright © 2020,2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PROJECT_ERROR_H
#define KAR_PROJECT_ERROR_H

#include <stddef.h>

#include "core/cursor.h"
#include "core/array.h"
#include "core/string.h"

// TODO: Возможно надо добавить имя модуля или другую привязку к модулю.
typedef struct {
	KarCursor cursor;
	int code;
	KarString* description;
} KarProjectError;

KarProjectError* kar_project_error_create(KarCursor* cursor, int code, const KarString* description);
void kar_project_error_free(KarProjectError* error);

void kar_project_error_print(const KarString* module_name, KarProjectError* error);

#endif // KAR_PROJECT_ERROR_H
