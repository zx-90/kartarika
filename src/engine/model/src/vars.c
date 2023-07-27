/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vars.h"

KarVars* kar_vars_create() {
    KAR_CREATE(vars, KarVars);

    vars->vartree = NULL;
    kar_vars_default_list_init(vars);
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

    return vars;
}

void kar_vars_free(KarVars* vars) {
    if (vars->vartree != NULL) {
        kar_vartree_free(vars->vartree);
    }
    kar_vars_default_list_tear_all(vars);
    KAR_FREE(vars)
}

KAR_ARRAY_CODE(vars_default_list, KarVars, KarVartree, default_list, kar_vartree_free)
