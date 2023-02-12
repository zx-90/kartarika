/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PROJECT_ERROR_LIST_H
#define KAR_PROJECT_ERROR_LIST_H

#include "project_error.h"

typedef struct {
	KarArray errors;
} KarProjectErrorList;

KarProjectErrorList* kar_project_error_list_create();
void kar_project_error_list_free(KarProjectErrorList* error);

KAR_TREE_HEADER(project_error_list, KarProjectErrorList, KarProjectError)

void kar_project_error_list_create_add(KarProjectErrorList* error_list, KarCursor* cursor, int code, const char* description);

#endif // KAR_PROJECT_ERROR_LIST_H
