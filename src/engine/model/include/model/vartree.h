/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_VARTREE_H
#define KAR_VARTREE_H

#include "core/set.h"
#include "core/array.h"

typedef enum {
	KAR_VARTYPE_UNKNOWN,
	
	KAR_VARTYPE_INTEGER,
	KAR_VARTYPE_UNSIGNED,
	KAR_VARTYPE_FLOAT,
	KAR_VARTYPE_STRING,
	KAR_VARTYPE_BOOL,
	
	KAR_VARTYPE_UNCLEAN,
	
	KAR_VARTYPE_CONST,
	KAR_VARTYPE_VARIABLE,
	KAR_VARTYPE_FUNCTION,
	
	KAR_VARTYPE_MODULE,
	KAR_VARTYPE_PACKET,
	KAR_VARTYPE_ROOT
} KarVartypeElement;

typedef struct KarVartreeStruct {
	char* name;
	KarVartypeElement type;
	KAR_SET_STRUCT(struct KarVartreeStruct) children;
	KAR_ARRAY_STRUCT(struct KarVartreeStruct) link;
	void* value;
} KarVartree;

KarVartree* kar_vartree_create();
void kar_vartree_free(KarVartree* vartree);

bool kar_vartree_less(KarVartree* vartree1, KarVartree* vartree2);
bool kar_vartree_equal(KarVartree* vartree1, KarVartree* vartree2);

KAR_SET_HEADER(vartree_child, KarVartree, KarVartree)
KAR_ARRAY_HEADER(vartree_link, KarVartree, KarVartree)

#endif // KAR_VARTREE_H
