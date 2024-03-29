/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree.h"

#include <string.h>

#include "core/alloc.h"
#include "core/string_list.h"
#include "core/string_builder.h"

#include "model/vartree_function_params.h"

static void(*nullFree)(KarVartree* item) = NULL;

static KarVartree* vartree_create(KarVartypeElement element) {
	KAR_CREATE(vartree, KarVartree);
	
	vartree->name = NULL;
    vartree->type = element;
	kar_vartree_args_init(vartree);
    kar_vartree_child_init(vartree);
    vartree->params = NULL;
    vartree->freeParams = NULL;

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

KarVartree* kar_vartree_create_class(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_CLASS, name);
}

KarVartree* kar_vartree_create_class_link(const KarString* name, KarVartree* type) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_CLASS_LINK, name);
    result->params = type;
	return result;
}

KarVartree* kar_vartree_create_function(const KarString* name, uint8_t modificators, const KarString* issueName, KarVartree** args, size_t args_count, KarVartree* return_type) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_FUNCTION, name);
	for (size_t i = 0; i < args_count; i++) {
		kar_vartree_args_add(result, args[i]);
	}
	result->params = kar_vartree_function_create(modificators, issueName, return_type);
    result->freeParams = &kar_vartree_function_free;
	return result;
}

KarVartree* kar_vartree_create_variable(const KarString* name, KarVartree* type) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_VARIABLE, name);
    result->params = type;
	return result;
}

//-----------------------------------------------------------------------------
// Описание констант.
//-----------------------------------------------------------------------------

// TODO: Для разных типов констант необходимы разные структуры, а не просто (void* value).
typedef struct {
    KarVartree* type;
    void* value;
} KarVartreeConstValue;

KarVartreeConstValue* kar_vartree_const_value_create(KarVartree* type, void* const_value) {
    KAR_CREATE(value, KarVartreeConstValue);

    value->type = type;
    value->value = const_value;

    return value;
}

void kar_vartree_const_value_free(void* ptr) {
    KarVartreeConstValue* value = (KarVartreeConstValue*)ptr;
    KAR_FREE(value);
}

KarVartree* kar_vartree_create_const(const KarString* name, KarVartree* type, void* value) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_CONST, name);
    result->params = kar_vartree_const_value_create(type, value);
    result->freeParams = &kar_vartree_const_value_free;
	return result;
}

//-----------------------------------------------------------------------------
// Описание констант закончено.
//-----------------------------------------------------------------------------

KarVartree* kar_vartree_create_unclean(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_UNCLEAN, name);
}

KarVartree* kar_vartree_create_unclean_class(KarVartree* type) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_UNCLEAN_CLASS, "Неопределённость");
	kar_vartree_args_add(result, type);
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

KarVartree* kar_vartree_create_0float(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_0FLOAT, name);
}

KarVartree* kar_vartree_create_0hex(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_0HEX, name);
}

KarVartree* kar_vartree_create_0integer(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_0INTEGER, name);
}

KarVartree* kar_vartree_create_bool(const KarString* name) {
	return vartree_create_name(KAR_VARTYPE_BOOL, name);
}

void kar_vartree_free(KarVartree* vartree) {
	if (vartree->name != NULL) {
		kar_string_free(vartree->name);
	}
	kar_vartree_args_clear(vartree);
	kar_vartree_child_clear(vartree);
    if (vartree->freeParams != NULL) {
        vartree->freeParams(vartree->params);
    }
}

KarString* kar_vartree_create_full_name_args(const KarString* name, KarVartree** args, size_t args_count) {
	if (name == NULL) {
		return NULL;
	}
	KarStringBuilder builder;
	kar_string_builder_init(&builder);

	kar_string_builder_push_string(&builder, name);
	kar_string_builder_push_string(&builder, "(");
	for (size_t i = 0; i < args_count; i++) {
		if (i != 0) {
			kar_string_builder_push_string(&builder, ",");
		}
		KarString* argName = kar_vartree_create_full_path(args[i]);
		kar_string_builder_push_string(&builder, argName);
		KAR_FREE(argName);
	}
	kar_string_builder_push_string(&builder, ")");

	return kar_string_builder_clear_get(&builder);
}

static KarString* create_full_name(KarVartree* var) {
	if (var->name == NULL) {
		return NULL;
	}
	if (var->type == KAR_VARTYPE_FUNCTION || var->type == KAR_VARTYPE_UNCLEAN_CLASS) {
		return kar_vartree_create_full_name_args(var->name, var->args.items, var->args.count);
	}
	return kar_string_create(var->name);
}

KarString* kar_vartree_create_full_path(KarVartree* var) {
	KarString* result = create_full_name(var);
	while (kar_vartree_child_parent(var) != NULL) {
		var = kar_vartree_child_parent(var);
		if (var->name == NULL) {
			continue;
		}

		KarString* newResult = kar_string_create_concat(".", result);
		KarString* added = create_full_name(var);
		KarString* newResult2 = kar_string_create_concat(added, newResult);

		KAR_FREE(added);
		KAR_FREE(result);
		KAR_FREE(newResult);
		result = newResult2;
	}
	return result;
}

bool kar_vartree_less(KarVartree* vartree1, KarVartree* vartree2) {
	if (vartree2->name == NULL) {
		return false;
	}
	if (vartree1->name == NULL) {
		return true;
	}
	if (kar_string_less(vartree1->name, vartree2->name)) {
		return true;
	}
	if (kar_string_less(vartree2->name, vartree1->name)) {
		return false;
	}
	for (size_t i = 0; i < kar_vartree_args_count(vartree1); i++) {
		if (kar_vartree_args_count(vartree2) <= i) {
			return false;
		}
		KarVartree* arg1 = kar_vartree_args_get(vartree1, i);
		KarVartree* arg2 = kar_vartree_args_get(vartree2, i);
		KarString* path1 = kar_vartree_create_full_path(arg1);
		KarString* path2 = kar_vartree_create_full_path(arg2);
		int cmpRes = strcmp(path1, path2);
		KAR_FREE(path1);
		KAR_FREE(path2);
		if (cmpRes < 0) {
			return true;
		}
		if (cmpRes > 0) {
			return false;
		}
	}
	if (kar_vartree_args_count(vartree1) < kar_vartree_args_count(vartree2)) {
		return true;
	}
	return false;
}

bool kar_vartree_equal(KarVartree* vartree1, KarVartree* vartree2) {
	if (vartree1->name == NULL && vartree2->name == NULL) {
		return true;
	}
	if (vartree1->name == NULL || vartree2->name == NULL) {
		return false;
	}
	if (!kar_string_equal(vartree1->name, vartree2->name)) {
		return false;
	}
	if (kar_vartree_args_count(vartree1) != kar_vartree_args_count(vartree2)) {
		return false;
	}
	for (size_t i = 0; i < kar_vartree_args_count(vartree1); i++) {
		if (kar_vartree_args_get(vartree1, i) != kar_vartree_args_get(vartree2, i)) {
			return false;
		}
	}
	return true;
}

KarVartree* kar_vartree_find(KarVartree* parent, const KarString* name) {
    for (size_t i = 0; i < kar_vartree_child_count(parent); i++) {
        KarVartree* child = kar_vartree_child_get(parent, i);
		if (kar_string_equal(child->name, name) && kar_vartree_args_count(child) == 0) {
            return child;
        }
    }
    return NULL;
}

KarVartree* kar_vartree_find_args(KarVartree *parent, const KarString* name, KarVartree** args, size_t args_count) {
	for (size_t i = 0; i < kar_vartree_child_count(parent); i++) {
		KarVartree* child = kar_vartree_child_get(parent, i);
		if (!kar_string_equal(child->name, name)) {
			continue;
		}
		if (kar_vartree_args_count(child) != args_count) {
			continue;
		}
		bool match = true;
		for (size_t i = 0; i < args_count; i++) {
			if (kar_vartree_args_get(child, i) != args[i]) {
				match = false;
				break;
			}
		}
		if (!match) {
			continue;
		}
		return child;
	}
	return NULL;
}
KAR_ARRAY_CODE(vartree_args, KarVartree, KarVartree, args, nullFree)

KAR_TREE_SET_CODE(vartree_child, KarVartree, children, kar_vartree_less, kar_vartree_equal, kar_vartree_free)

KarVartreeFunctionParams* kar_vartree_get_function_params(KarVartree* vartree) {
	if (vartree->type != KAR_VARTYPE_FUNCTION) {
		return NULL;
	}
	return (KarVartreeFunctionParams*)vartree->params;
}

KarVartree* kar_vartree_get_unclean_class(KarVartree* vartree) {
	if (vartree->type != KAR_VARTYPE_UNCLEAN_CLASS) {
		return NULL;
	}
	return kar_vartree_args_get(vartree, 0);
}
