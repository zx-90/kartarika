/* Copyright © 2025 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree_class.h"

KarVartree* kar_vartree_create_class(KarVartree*parent, const KarString* name) {
    return kar_vartree_create_predefined_class(parent, KAR_CLASS_TYPE_CLASS, name);
}

KarVartree* kar_vartree_create_predefined_class(KarVartree *parent, KarClassType type, const KarString* name) {
	if (parent == NULL || parent->type != KAR_VARTYPE_PACKAGE) {
		return NULL;
	}
	if (kar_vartree_find(parent, name) != NULL) {
		return NULL;
	}
	KarVartree* result = kar_vartree_create_name(KAR_VARTYPE_CLASS, name);
	result->params = (void*)type;
	kar_vartree_child_add(parent, result);
	return result;
}

KarVartree* kar_vartree_create_class_link(KarVartree* parent, const KarString* name, KarVartree* type) {
	if (parent == NULL || parent->type != KAR_VARTYPE_PACKAGE) {
		return NULL;
	}
	if (kar_vartree_find(parent, name) != NULL) {
		return NULL;
	}
	KarVartree* result = kar_vartree_create_name(KAR_VARTYPE_CLASS_LINK, name);
	result->params = type;
	kar_vartree_child_add(parent, result);
	return result;
}

KarClassType kar_vartree_get_class_type(const KarVartree* vartree) {
	if (vartree->type != KAR_VARTYPE_CLASS) {
		return KAR_CLASS_TYPE_UNKNOWN;
	}
	return (KarClassType)vartree->params;
}

