/* Copyright © 2025 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree_template.h"

KarVartree* kar_vartree_create_unclean(KarVartree *parent, const KarString* name) {
	if (parent == NULL || parent->type != KAR_VARTYPE_PACKAGE) {
		return NULL;
	}
	if (kar_vartree_find(parent, name) != NULL) {
		return NULL;
	}
	KarVartree* result = kar_vartree_create_name(KAR_VARTYPE_UNCLEAN, name);
	kar_vartree_child_add(parent, result);
	return result;
}

KarVartree* kar_vartree_create_unclean_class(KarVartree *parent, KarVartree* type) {
	if (parent == NULL || parent->type != KAR_VARTYPE_PACKAGE) {
		return NULL;
	}
	// TODO: Слово "Неопределённость" необходимо вынести в константу.
	//       Отсюда, из анализатора и возможно ещё из каких-то мест.
	if (kar_vartree_find_args(parent, "Неопределённость", &type, 1) != NULL) {
		return NULL;
	}
	KarVartree* result = kar_vartree_create_name(KAR_VARTYPE_UNCLEAN_CLASS, "Неопределённость");
	kar_vartree_args_add(result, type);
	kar_vartree_child_add(parent, result);
	return result;
}

