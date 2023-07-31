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

static KarVartree* vartree_create(KarVartypeElement element) {
	KAR_CREATE(vartree, KarVartree);
	
	vartree->name = NULL;
    vartree->type = element;
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
	return vartree_create_name(KAR_VARTYPE_MODULE, name);
}

KarVartree* kar_vartree_create_class_link(const KarString* name, KarVartree* type) {
	KarVartree* result = vartree_create_name(KAR_VARTYPE_MODULE_LINK, name);
    result->params = type;
	return result;
}

KarVartree* kar_vartree_create_function(const KarString* name, const KarString* issueName, KarVartree** args, size_t args_count, KarVartree* return_type) {
    KarVartree* result = vartree_create_name(KAR_VARTYPE_FUNCTION, kar_vartree_create_full_function_name(name, args, args_count));
    result->params = kar_vartree_function_create(issueName, args, args_count, return_type);
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

KarVartree* kar_vartree_create_unclean_module(KarVartree* type) {
    KarVartree* result = vartree_create_name(KAR_VARTYPE_UNCLEAN_MODULE, "?");
    result->params = type;
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
    if (vartree->freeParams != NULL) {
        vartree->freeParams(vartree->params);
    }
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

KarString* kar_vartree_create_full_path(KarVartree* var) {
	KarString* result = kar_string_create(var->name);
	while (kar_vartree_child_parent(var) != NULL) {
		var = kar_vartree_child_parent(var);
		if (var->name == NULL) {
			continue;
		}
		KarString* newResult = kar_string_create_concat(".", result);
		KarString* newResult2 = kar_string_create_concat(var->name, newResult);

		KAR_FREE(result);
		KAR_FREE(newResult);
		result = newResult2;
	}
	return result;
}

KarVartree* kar_vartree_find(KarVartree* parent, const KarString* name) {
    for (size_t i = 0; i < kar_vartree_child_count(parent); i++) {
        KarVartree* child = kar_vartree_child_get(parent, i);
        if (kar_string_equal(child->name, name)) {
            return child;
        }
    }
    return NULL;
}

KAR_TREE_SET_CODE(vartree_child, KarVartree, children, kar_vartree_less, kar_vartree_equal, kar_vartree_free)

KarVartreeFunctionParams* kar_vartree_get_function_params(KarVartree* vartree) {
	if (vartree->type != KAR_VARTYPE_FUNCTION) {
		return NULL;
	}
	return (KarVartreeFunctionParams*)vartree->params;
}
