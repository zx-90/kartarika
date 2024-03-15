/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_VARS_H
#define KAR_VARS_H

#include "vartree.h"

typedef struct {
    KarVartree* vartree;
    KAR_ARRAY_STRUCT(KarVartree) default_list;
    struct {
		KarVartree* nullType;

		KarVartree* boolType;

		KarVartree* decimalType;
		KarVartree* hexadecimalType;

		KarVartree* int8Type;
        KarVartree* int16Type;
        KarVartree* int32Type;
        KarVartree* int64Type;
        KarVartree* intType;

        KarVartree* unsigned8Type;
        KarVartree* unsigned16Type;
        KarVartree* unsigned32Type;
        KarVartree* unsigned64Type;
        KarVartree* unsignedType;

        KarVartree* float32Type;
        KarVartree* float64Type;
        KarVartree* floatType;

        KarVartree* stringType;

		KarVartree* unclean;

		KarVartree* uncleanBool;

		KarVartree* uncleanInt8;
		KarVartree* uncleanInt16;
		KarVartree* uncleanInt32;
		KarVartree* uncleanInt64;
		KarVartree* uncleanInt;

		KarVartree* uncleanUnsigned8;
		KarVartree* uncleanUnsigned16;
		KarVartree* uncleanUnsigned32;
		KarVartree* uncleanUnsigned64;
		KarVartree* uncleanUnsigned;

		KarVartree* uncleanFloat32;
		KarVartree* uncleanFloat64;
		KarVartree* uncleanFloat;

		KarVartree* uncleanString;

	} standard;
} KarVars;

KarVars* kar_vars_create();
void kar_vars_free(KarVars* vars);

KarVartree* kar_vars_find(KarVars* vars, KarString* name);
KarVartree* kar_vars_find_args(KarVars* vars, KarString* name, KarVartree** args, size_t args_count);

KAR_ARRAY_HEADER(vars_default_list, KarVars, KarVartree)

#endif // KAR_VARS_H
