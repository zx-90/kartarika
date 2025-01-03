/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree_function.h"

#include "core/string_builder.h"
#include "model/vartree.h"

static KarVartreeFunction* kar_vartree_function_create(uint8_t modificators, const KarString* issueName, KarVartree* return_type) {
	KAR_CREATE(value, KarVartreeFunction);

	value->modificators = modificators;
	value->issueName = kar_string_create(issueName);
	value->returnType = return_type;

	return value;
}

static void kar_vartree_function_free(void* ptr) {
	KarVartreeFunction* value = (KarVartreeFunction*)ptr;
	if (value->issueName != NULL) {
		KAR_FREE(value->issueName);
	}
	KAR_FREE(value);
}

KarVartree* kar_vartree_create_function(KarVartree *parent, const KarString* name, uint8_t modificators, const KarString* issueName, KarVartree** args, size_t args_count, KarVartree* return_type) {
	// TODO: Здесь есть условие `parent->type != KAR_VARTYPE_PACKAGE`
	//       Его тут не должно быть, так как в пакет засунуты
	//       конструкторы с параметрами, например: Целое8(Строка)
	//       Необходимо перенести конструкторы в классы, а это подусловие удалить.
	if (parent == NULL ||
		(parent->type != KAR_VARTYPE_CLASS &&
		 parent->type != KAR_VARTYPE_UNCLEAN &&
		 parent->type != KAR_VARTYPE_UNCLEAN_CLASS &&
		 parent->type != KAR_VARTYPE_PACKAGE)
	) {
		return NULL;
	}
	if (kar_vartree_find_args(parent, name, args, args_count) != NULL) {
		return NULL;
	}
	KarVartree* result = kar_vartree_create_name(KAR_VARTYPE_FUNCTION, name);
	for (size_t i = 0; i < args_count; i++) {
		kar_vartree_args_add(result, args[i]);
	}
	result->params = kar_vartree_function_create(modificators, issueName, return_type);
	result->freeParams = &kar_vartree_function_free;
	kar_vartree_child_add(parent, result);
	return result;
}

KarVartreeFunction* kar_vartree_get_function_params(KarVartree* vartree) {
	if (vartree->type != KAR_VARTYPE_FUNCTION) {
		return NULL;
	}
	return (KarVartreeFunction*)vartree->params;
}

bool kar_vartree_function_is_static(uint8_t modificators) {
	return (modificators & 1) == 0;
}

bool kar_vartree_function_is_dynamic(uint8_t modificators) {
	return (modificators & 1) == 1;
}

bool kar_vartree_function_is_private(uint8_t modificators) {
	return (modificators & (3 << 1)) == 0 << 1;
}

bool kar_vartree_function_is_protected(uint8_t modificators) {
	return (modificators & (3 << 1)) == 1 << 1;
}

bool kar_vartree_function_is_public(uint8_t modificators) {
	return (modificators & (3 << 1)) == 2 << 1;
}

bool kar_vartree_function_is_finalized(uint8_t modificators) {
	return (modificators & (1 << 3)) == 0 << 3;
}

bool kar_vartree_function_is_heritable(uint8_t modificators) {
	return (modificators & (1 << 3)) == 1 << 3;
}

bool kar_vartree_function_is_overloaded(uint8_t modificators) {
	return (modificators & (1 << 4)) == 1 << 4;
}

