/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_VARTREE_H
#define KAR_VARTREE_H

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

typedef struct {
	char* name;
	KarVartypeElement type;
	KarArray children;
	KarArray link;
	void* value;
} KarVartree;

#define kar_vartree_child(vartree, num) ((KarVartree*)(vartree)->children.items[(num)])
#define kar_vartree_link(vartree, num) ((KarVartree*)(vartree)->link.items[(num)])

KarVartree* kar_vartree_create();
void kar_vartree_free(KarVartree* vartree);

KAR_TREE_HEADER(vartree_child, KarVartree, KarVartree)
KAR_TREE_HEADER(vartree_link, KarVartree, KarVartree)

#endif // KAR_VARTREE_H
