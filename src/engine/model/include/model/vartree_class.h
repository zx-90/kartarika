/* Copyright © 2025 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_VARTREE_CLASS_H
#define KAR_VARTREE_CLASS_H

#include "model/vartree.h"

typedef enum {
	KAR_CLASS_TYPE_UNKNOWN,
	KAR_CLASS_TYPE_BOOL,
	KAR_CLASS_TYPE_0INTEGER,
	KAR_CLASS_TYPE_0HEX,
	KAR_CLASS_TYPE_0FLOAT,
	KAR_CLASS_TYPE_INTEGER8,
	KAR_CLASS_TYPE_INTEGER16,
	KAR_CLASS_TYPE_INTEGER32,
	KAR_CLASS_TYPE_INTEGER64,
	KAR_CLASS_TYPE_UNSIGNED8,
	KAR_CLASS_TYPE_UNSIGNED16,
	KAR_CLASS_TYPE_UNSIGNED32,
	KAR_CLASS_TYPE_UNSIGNED64,
	KAR_CLASS_TYPE_FLOAT32,
	KAR_CLASS_TYPE_FLOAT64,
	KAR_CLASS_TYPE_STRING,
    KAR_CLASS_TYPE_CLASS,
	KAR_CLASS_TYPE_CUSTOM
} KarClassType;

KarVartree* kar_vartree_create_class(KarVartree* parent, const KarString* name);
KarVartree* kar_vartree_create_predefined_class(KarVartree* parent, KarClassType type, const KarString* name);
KarVartree* kar_vartree_create_class_link(KarVartree* parent, const KarString* name, KarVartree* type);

KarClassType kar_vartree_get_class_type(const KarVartree* vartree);

#endif // KAR_VARTREE_CLASS_H
