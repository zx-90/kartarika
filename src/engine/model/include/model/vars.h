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
    } standard;
} KarVars;

KarVars* kar_vars_create();
void kar_vars_free(KarVars* vars);

KarVartree* kar_vars_find(KarVars* vars, KarString* name);

KAR_ARRAY_HEADER(vars_default_list, KarVars, KarVartree)

#endif // KAR_VARS_H
