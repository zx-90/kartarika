/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree.h"

#include <string.h>

#include "core/alloc.h"

static void(*link_free)(KarVartree* item) = NULL;

KarVartree* kar_vartree_create() {
	KAR_CREATE(vartree, KarVartree);
	
	vartree->name = NULL;
	vartree->type = KAR_VARTYPE_UNKNOWN;
	kar_vartree_child_init(vartree);
	kar_vartree_link_init(vartree);
	vartree->value = NULL;
	
	return vartree;
}

void kar_vartree_free(KarVartree* vartree) {
	if (vartree->name != NULL) {
		KAR_FREE(vartree->name);
	}
	kar_vartree_child_clear(vartree);
	kar_vartree_link_clear(vartree);
	// TODO: необходимо правильное удаление.
	if (vartree->value != NULL) {
		KAR_FREE(vartree->value);
	}
	KAR_FREE(vartree);
}

bool kar_vartree_less(KarVartree* vartree1, KarVartree* vartree2) {
	return strcmp(vartree1->name, vartree2->name) < 0;
}

bool kar_vartree_equal(KarVartree* vartree1, KarVartree* vartree2) {
	return strcmp(vartree1->name, vartree2->name) == 0;
}

KAR_SET_CODE(vartree_child, KarVartree, KarVartree, children, kar_vartree_less, kar_vartree_equal, kar_vartree_free)
KAR_ARRAY_CODE(vartree_link, KarVartree, KarVartree, link, link_free)
