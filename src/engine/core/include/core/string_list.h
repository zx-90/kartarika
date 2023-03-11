/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_STRING_LIST_H
#define KAR_STRING_LIST_H

#include "string.h"
#include "array.h"

typedef struct {
	KAR_ARRAY_STRUCT(KarString) list;
} KarStringList;

KarStringList* kar_string_list_create();
void kar_string_list_free(KarStringList* list);

KAR_ARRAY_HEADER(string_list, KarStringList, KarString)

#endif // KAR_STRING_LIST_H
