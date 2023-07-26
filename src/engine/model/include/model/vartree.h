/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_VARTREE_H
#define KAR_VARTREE_H

#include "core/set.h"
#include "core/array.h"
#include "core/string_list.h"

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
	KAR_VARTYPE_UNCLEAN_MODULE,
	KAR_VARTYPE_UNCLEAN,
	
	KAR_VARTYPE_CONST,
	KAR_VARTYPE_VARIABLE,
	KAR_VARTYPE_FUNCTION,
	
	KAR_VARTYPE_MODULE_LINK,
	KAR_VARTYPE_MODULE,
	
	KAR_VARTYPE_PACKET,
	KAR_VARTYPE_ROOT
} KarVartypeElement;

typedef struct KarVartreeStruct {
    KarString* name;
    bool initialized;
    KarString* issueName;
    KarVartypeElement type;
	KAR_SET_STRUCT(struct KarVartreeStruct) children;
	KAR_ARRAY_STRUCT(struct KarVartreeStruct) link;
	// TODO: value должно зависеть от типа.
	void* value;
} KarVartree;

KarVartree* kar_vartree_create_root();
KarVartree* kar_vartree_create_package(const KarString* name);

KarVartree* kar_vartree_create_module(const KarString* name);
KarVartree* kar_vartree_create_module_link(const KarString* name, KarVartree* type);

KarString* kar_vartree_create_full_function_name(const KarString* name, KarVartree** args, size_t args_count);
KarVartree* kar_vartree_create_function(const KarString* name, const KarString* libName, KarVartree** args, size_t args_count, KarVartree* return_type);
KarVartree* kar_vartree_create_variable(const KarString* name, KarVartree* type);
// TODO: Сделать функцию безопасной для поля void. Скорее надо будет разбить на несколько функций.
KarVartree* kar_vartree_create_const(const KarString* name, KarVartree* type, void* value);

KarVartree* kar_vartree_create_unclean(const KarString* name);
KarVartree* kar_vartree_create_unclean_module(KarVartree* type);

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

KarVartree* kar_vertree_find(KarVartree *parent, const KarString* name);

KAR_SET_HEADER(vartree_child, KarVartree, KarVartree)
KAR_ARRAY_HEADER(vartree_link, KarVartree, KarVartree)

#endif // KAR_VARTREE_H
