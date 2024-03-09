/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vars.h"

static void clean_standard(KarVars* vars) {
	vars->standard.nullType = NULL;

	vars->standard.boolType = NULL;
	vars->standard.int8Type = NULL;
	vars->standard.int16Type = NULL;
	vars->standard.int32Type = NULL;
	vars->standard.int64Type = NULL;
	vars->standard.intType = NULL;
	vars->standard.unsigned8Type = NULL;
	vars->standard.unsigned16Type = NULL;
	vars->standard.unsigned32Type = NULL;
	vars->standard.unsigned64Type = NULL;
	vars->standard.unsignedType = NULL;
	vars->standard.float32Type = NULL;
	vars->standard.float64Type = NULL;
	vars->standard.floatType = NULL;
	vars->standard.stringType = NULL;

	vars->standard.uncleanBool = NULL;
	vars->standard.uncleanInt8 = NULL;
	vars->standard.uncleanInt16 = NULL;
	vars->standard.uncleanInt32 = NULL;
	vars->standard.uncleanInt64 = NULL;
	vars->standard.uncleanInt = NULL;
	vars->standard.uncleanUnsigned8 = NULL;
	vars->standard.uncleanUnsigned16 = NULL;
	vars->standard.uncleanUnsigned32 = NULL;
	vars->standard.uncleanUnsigned64 = NULL;
	vars->standard.uncleanUnsigned = NULL;
	vars->standard.uncleanFloat32 = NULL;
	vars->standard.uncleanFloat64 = NULL;
	vars->standard.uncleanFloat = NULL;
	vars->standard.uncleanString = NULL;
}

KarVars* kar_vars_create() {
    KAR_CREATE(vars, KarVars);

    vars->vartree = NULL;
    kar_vars_default_list_init(vars);
	clean_standard(vars);

	return vars;
}

void kar_vars_free(KarVars* vars) {
    if (vars->vartree != NULL) {
        kar_vartree_free(vars->vartree);
    }
    kar_vars_default_list_tear_all(vars);
	clean_standard(vars);
    KAR_FREE(vars)
}

KarVartree* kar_vars_find(KarVars* vars, KarString* name) {
	if (name == NULL) {
		return NULL;
	}
    for (size_t i = 0; i < kar_vars_default_list_count(vars); i++) {
        KarVartree* result = kar_vartree_find(kar_vars_default_list_get(vars, i), name);
        if (result != NULL) {
            return result;
        }
    }
    return NULL;
}

KAR_ARRAY_CODE(vars_default_list, KarVars, KarVartree, default_list, kar_vartree_free)
