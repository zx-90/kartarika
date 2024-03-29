/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PROJECT_ERROR_LIST_H
#define KAR_PROJECT_ERROR_LIST_H

#include "project_error.h"

typedef struct {
	KAR_ARRAY_STRUCT(KarProjectError) errors;
} KarProjectErrorList;

KarProjectErrorList* kar_project_error_list_create();
void kar_project_error_list_free(KarProjectErrorList* error);

KAR_ARRAY_HEADER(project_error_list, KarProjectErrorList, KarProjectError)

void kar_project_error_list_create_add(KarProjectErrorList* error_list, KarString* moduleName, KarCursor* cursor, int code, const KarString* description);

void kar_project_error_list_print(KarProjectErrorList* errors);
KarString* kar_project_error_list_create_string(KarProjectErrorList *errors);

#endif // KAR_PROJECT_ERROR_LIST_H
