/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree.h"

#include <string.h>

#include "core/alloc.h"
#include "core/string_list.h"

static void(*link_free)(KarVartree* item) = NULL;

static KarVartree* vartree_create(KarVartypeElement element) {
	KAR_CREATE(vartree, KarVartree);
	
	vartree->name = NULL;
	vartree->type = element;
	kar_vartree_child_init(vartree);
	kar_vartree_link_init(vartree);
	vartree->value = NULL;
	
	return vartree;
}

static KarVartree* vartree_create_name(KarVartypeElement element, const KarString* name) {
	KarVartree* result = vartree_create(element);
	result->name = kar_string_create(name);
	return result;
}

KarVartree* kar_vartree_create_root() {
	return vartree_create(KAR_VARTYPE_ROOT);
}

KarVartree* kar_vartree_create_package(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_PACKET, name);
}

KarVartree* kar_vartree_create_module(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_MODULE, name);
}

KarVartree* kar_vartree_create_module_link(const KarString* name, KarVartree* type) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_MODULE_LINK, name);
	kar_vartree_link_add(result, type);
	return result;
}

KarVartree* kar_vartree_create_function(const KarString* name, KarVartree** args, size_t args_count, KarVartree* return_type) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_FUNCTION, name);
	kar_vartree_link_add(result, return_type);
	for (size_t i = 0; i < args_count; i++) {
		kar_vartree_link_add(result, args[i]);
	}
	return result;
}

KarVartree* kar_vartree_create_variable(const KarString* name, KarVartree* type) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_VARIABLE, name);
	kar_vartree_link_add(result, type);
	return result;
}

KarVartree* kar_vartree_create_const(const KarString* name, KarVartree* type, void* value) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_CONST, name);
	kar_vartree_link_add(result, type);
	result->value = value;
	return result;
}

KarVartree* kar_vartree_create_unclean(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_UNCLEAN, name);
}

KarVartree* kar_vartree_create_unclean_module(KarVartree* type) {
	KarVartree* result = vartree_create(KAR_VARTYPE_UNCLEAN_MODULE);
	kar_vartree_link_add(result, type);
	return result;
}

KarVartree* kar_vartree_create_string(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_STRING, name);
}

KarVartree* kar_vartree_create_float64(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_FLOAT64, name);
}

KarVartree* kar_vartree_create_float32(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_FLOAT32, name);
}

KarVartree* kar_vartree_create_unsigned64(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_UNSIGNED64, name);
}

KarVartree* kar_vartree_create_unsigned32(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_UNSIGNED32, name);
}

KarVartree* kar_vartree_create_unsigned16(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_UNSIGNED16, name);
}

KarVartree* kar_vartree_create_unsigned8(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_UNSIGNED8, name);
}

KarVartree* kar_vartree_create_integer64(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_INTEGER64, name);
}

KarVartree* kar_vartree_create_integer32(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_INTEGER32, name);
}

KarVartree* kar_vartree_create_integer16(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_INTEGER16, name);
}

KarVartree* kar_vartree_create_integer8(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_INTEGER8, name);
}

KarVartree* kar_vartree_create_bool(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_BOOL, name);
}

void kar_vartree_free(KarVartree* vartree) {
	if (vartree->name != NULL) {
		kar_string_free(vartree->name);
	}
	kar_vartree_child_clear(vartree);
	kar_vartree_link_clear(vartree);
	// TODO: необходимо правильное удаление vartree->value.
	KAR_FREE(vartree);
}

bool kar_vartree_less(KarVartree* vartree1, KarVartree* vartree2) {
	if (vartree2->name == NULL) {
		return false;
	}
	if (vartree1->name == NULL) {
		return true;
	}
	return kar_string_less(vartree1->name, vartree2->name);
}

bool kar_vartree_equal(KarVartree* vartree1, KarVartree* vartree2) {
	if (vartree1->name == NULL && vartree2->name == NULL) {
		return true;
	}
	if (vartree1->name == NULL || vartree2->name == NULL) {
		return false;
	}
	return kar_string_equal(vartree1->name, vartree2->name);
}

KAR_SET_CODE(vartree_child, KarVartree, KarVartree, children, kar_vartree_less, kar_vartree_equal, kar_vartree_free)
KAR_ARRAY_CODE(vartree_link, KarVartree, KarVartree, link, link_free)
