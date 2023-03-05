/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/string_list.h"

KarStringList* kar_string_list_create() {
	KAR_CREATE(list, KarStringList);
	kar_string_list_init(list);
	return list;
}

KarStringList* kar_string_list_create_stamp(char** list, size_t count) {
	KarStringList* result = kar_string_list_create();
	for (size_t i = 0; i < count; i++) {
		kar_string_list_add(result, list[i]);
	}
	return result;
}

void kar_string_list_free(KarStringList* list) {
	kar_string_list_clear(list);
	KAR_FREE(list);
}

KAR_ARRAY_CODE(string_list, KarStringList, KarString, list, kar_string_free)
