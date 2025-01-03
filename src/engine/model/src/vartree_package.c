/* Copyright © 2025 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree_package.h"

KarVartree* kar_vartree_create_root() {
	return kar_vartree_create(KAR_VARTYPE_ROOT);
}

KarVartree* kar_vartree_create_package(KarVartree *parent, const KarString* name) {
	if (parent == NULL || (parent->type != KAR_VARTYPE_ROOT && parent->type != KAR_VARTYPE_PACKAGE)) {
		return NULL;
	}
	if (kar_vartree_find(parent, name) != NULL) {
		return NULL;
	}
	KarVartree* package = kar_vartree_create_name(KAR_VARTYPE_PACKAGE, name);
	kar_vartree_child_add(parent, package);
	return package;
}

