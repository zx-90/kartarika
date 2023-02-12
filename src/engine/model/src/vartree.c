/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree.h"

#include "core/alloc.h"

KarVartree* kar_vartree_create() {
	KAR_CREATE(vartree, KarVartree);
	
	vartree->name = NULL;
	vartree->type = KAR_VARTYPE_UNKNOWN;
	kar_array_init(&vartree->children);
	kar_array_init(&vartree->link);
	vartree->value = NULL;
	
	return vartree;
}

void kar_vartree_free(KarVartree* vartree) {
	if (vartree->name != NULL) {
		KAR_FREE(vartree->name);
	}
	kar_array_clear(&vartree->children, (KarArrayFreeFn*)&kar_vartree_free);
	kar_array_clear(&vartree->link, NULL);
	// TODO: необходимо правильное удаление.
	if (vartree->value != NULL) {
		KAR_FREE(vartree->value);
	}
	KAR_FREE(vartree);
}

KAR_TREE_CODE(cartree_child, KarVartree, KarVartree, children, &kar_vartree_free)
KAR_TREE_CODE(vartree_link, KarVartree, KarVartree, link, NULL)
