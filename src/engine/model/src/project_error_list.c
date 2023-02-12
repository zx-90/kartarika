/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/project_error_list.h"

KarProjectErrorList* kar_project_error_list_create() {
	KAR_CREATE(project_error_list, KarProjectErrorList);
	kar_project_error_list_init(project_error_list);
	return project_error_list;
}

void kar_project_error_list_free(KarProjectErrorList* error_list) {
	kar_project_error_list_clear(error_list);
}

KAR_TREE_CODE(project_error_list, KarProjectErrorList, KarProjectError, errors, kar_project_error_free)

void kar_project_error_list_create_add(KarProjectErrorList* error_list, KarCursor* cursor, int code, const char* description) {
	kar_project_error_list_add(error_list, kar_project_error_create(cursor, code, description));
}

