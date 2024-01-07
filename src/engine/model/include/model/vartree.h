/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_VARTREE_H
#define KAR_VARTREE_H

#include "core/tree_set.h"
#include "core/array.h"
#include "core/string_list.h"

#include "vartree_function_params.h"

typedef enum {
	KAR_VARTYPE_UNKNOWN,
	
	KAR_VARTYPE_BOOL,
	KAR_VARTYPE_INTEGER8,
	KAR_VARTYPE_INTEGER16,
	KAR_VARTYPE_INTEGER32,
	KAR_VARTYPE_INTEGER64,
	KAR_VARTYPE_UNSIGNED8,
	KAR_VARTYPE_UNSIGNED16,
	KAR_VARTYPE_UNSIGNED32,
	KAR_VARTYPE_UNSIGNED64,
	KAR_VARTYPE_FLOAT32,
	KAR_VARTYPE_FLOAT64,
	KAR_VARTYPE_STRING,
	
	// TODO: Сделать различие между шаблоном класса и классом.
	// TODO: Представить Неопределённость как шаблон класса.
	KAR_VARTYPE_UNCLEAN,
	KAR_VARTYPE_UNCLEAN_CLASS,

	KAR_VARTYPE_CONST,
	KAR_VARTYPE_VARIABLE,
	KAR_VARTYPE_FUNCTION,
	
	KAR_VARTYPE_CLASS_LINK,
	KAR_VARTYPE_CLASS,
	
	KAR_VARTYPE_PACKET,
	KAR_VARTYPE_ROOT
} KarVartypeElement;

typedef struct KarVartreeStruct {
    KarString* name;
    KarVartypeElement type;
    KAR_TREE_SET_STRUCT(struct KarVartreeStruct) children;
    void* params;
    void (*freeParams)(void*);
} KarVartree;

KarVartree* kar_vartree_create_root();
KarVartree* kar_vartree_create_package(const KarString* name);

KarVartree* kar_vartree_create_class(const KarString* name);
KarVartree* kar_vartree_create_class_link(const KarString* name, KarVartree* type);

KarVartree* kar_vartree_create_function(const KarString* name, uint8_t modificators, const KarString* libName, KarVartree** args, size_t args_count, KarVartree* return_type);
KarVartree* kar_vartree_create_variable(const KarString* name, KarVartree* type);
// TODO: Сделать функцию безопасной для поля void. Скорее надо будет разбить на несколько функций.
KarVartree* kar_vartree_create_const(const KarString* name, KarVartree* type, void* value);

KarVartree* kar_vartree_create_unclean(const KarString* name);
KarVartree* kar_vartree_create_unclean_class(KarVartree* type);

KarVartree* kar_vartree_create_string(const KarString* name);
KarVartree* kar_vartree_create_float64(const KarString* name);
KarVartree* kar_vartree_create_float32(const KarString* name);
KarVartree* kar_vartree_create_unsigned64(const KarString* name);
KarVartree* kar_vartree_create_unsigned32(const KarString* name);
KarVartree* kar_vartree_create_unsigned16(const KarString* name);
KarVartree* kar_vartree_create_unsigned8(const KarString* name);
KarVartree* kar_vartree_create_integer64(const KarString* name);
KarVartree* kar_vartree_create_integer32(const KarString* name);
KarVartree* kar_vartree_create_integer16(const KarString* name);
KarVartree* kar_vartree_create_integer8(const KarString* name);
KarVartree* kar_vartree_create_bool(const KarString* name);

void kar_vartree_free(KarVartree* vartree);

bool kar_vartree_less(KarVartree* vartree1, KarVartree* vartree2);
bool kar_vartree_equal(KarVartree* vartree1, KarVartree* vartree2);

KarString* kar_vartree_create_full_path(KarVartree* var);
KarVartree* kar_vartree_find(KarVartree *parent, const KarString* name);

KAR_TREE_SET_HEADER(vartree_child, KarVartree)
KAR_ARRAY_HEADER(vartree_link, KarVartree, KarVartree)

KarVartreeFunctionParams* kar_vartree_get_function_params(KarVartree* vartree);
KarVartree* kar_vartree_get_unclean_class(KarVartree* vartree);

#endif // KAR_VARTREE_H
