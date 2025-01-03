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

typedef enum {
	KAR_VARTYPE_UNKNOWN,
	
	/*KAR_VARTYPE_BOOL,
	KAR_VARTYPE_0INTEGER,
	KAR_VARTYPE_0HEX,
	KAR_VARTYPE_0FLOAT,
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
	KAR_VARTYPE_STRING,*/
	
	// TODO: Сделать различие между шаблоном класса и классом.
	// TODO: Представить Неопределённость как шаблон класса.
	KAR_VARTYPE_UNCLEAN,
	KAR_VARTYPE_UNCLEAN_CLASS,

	KAR_VARTYPE_CONST,
	KAR_VARTYPE_VARIABLE,
	KAR_VARTYPE_FUNCTION,
	
	KAR_VARTYPE_CLASS_LINK,
	KAR_VARTYPE_CLASS,
	
	KAR_VARTYPE_PACKAGE,
	KAR_VARTYPE_ROOT
} KarVartypeElement;

typedef struct KarVartreeStruct {
    KarString* name;
    KarVartypeElement type;
	KAR_ARRAY_STRUCT(struct KarVartreeStruct) args;
	KAR_TREE_SET_STRUCT(struct KarVartreeStruct) children;
    void* params;
    void (*freeParams)(void*);
	void* generatorParams;
	void (*freeGeneratorParams)(void*);
} KarVartree;

KarVartree* kar_vartree_create(KarVartypeElement element);
KarVartree* kar_vartree_create_name(KarVartypeElement element, const KarString* name);

void kar_vartree_free(KarVartree* vartree);

KarString* kar_vartree_create_full_name_args(const KarString* name, KarVartree** args, size_t args_count);
KarString* kar_vartree_create_full_path(KarVartree* var);

bool kar_vartree_less(KarVartree* vartree1, KarVartree* vartree2);
bool kar_vartree_equal(KarVartree* vartree1, KarVartree* vartree2);

KarVartree* kar_vartree_find(KarVartree *parent, const KarString* name);
KarVartree* kar_vartree_find_args(KarVartree *parent, const KarString* name, KarVartree** args, size_t args_count);

KAR_ARRAY_HEADER(vartree_args, KarVartree, KarVartree)

KAR_TREE_SET_HEADER(vartree_child, KarVartree)

KarVartree* kar_vartree_get_unclean_class(KarVartree* vartree);

#endif // KAR_VARTREE_H
