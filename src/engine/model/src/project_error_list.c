/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/project_error_list.h"

#include "core/string_builder.h"

KarProjectErrorList* kar_project_error_list_create() {
	KAR_CREATE(project_error_list, KarProjectErrorList);
	kar_project_error_list_init(project_error_list);
	return project_error_list;
}

void kar_project_error_list_free(KarProjectErrorList* error_list) {
	kar_project_error_list_clear(error_list);
}

KAR_ARRAY_CODE(project_error_list, KarProjectErrorList, KarProjectError, errors, kar_project_error_free)

void kar_project_error_list_create_add(KarProjectErrorList* error_list, KarString* moduleName, KarCursor* cursor, int code, const KarString* description) {
    kar_project_error_list_add(error_list, kar_project_error_create(moduleName, cursor, code, description));
}

void kar_project_error_list_print(KarProjectErrorList *errors) {
    for (size_t i = 0; i < kar_project_error_list_count(errors); ++i) {
        kar_project_error_print(kar_project_error_list_get(errors, i));
    }
}

KarString* kar_project_error_list_create_string(KarProjectErrorList* errors) {
	KarStringBuilder builder;
	kar_string_builder_init(&builder);
	for (size_t i = 0; i < kar_project_error_list_count(errors); ++i) {
		KarString* str = kar_project_error_create_string(kar_project_error_list_get(errors, i));
		kar_string_builder_push_string(&builder, str);
		KAR_FREE(str);
	}
	return kar_string_builder_clear_get(&builder);
}
