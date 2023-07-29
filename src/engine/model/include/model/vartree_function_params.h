/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_VARTREE_FUNCTION_PARAMS_H
#define KAR_VARTREE_FUNCTION_PARAMS_H

#include "core/array.h"
#include "core/string.h"

struct KarVartreeStruct;
typedef struct KarVartreeStruct KarVartree;

typedef struct {
    KarString* issueName;
	KAR_ARRAY_STRUCT(KarVartree) args;
	KarVartree* returnType;
} KarVartreeFunctionParams;

KarVartreeFunctionParams* kar_vartree_function_create(const KarString* issueName, KarVartree** args, size_t args_count, KarVartree* return_type);
void kar_vartree_function_free(void* ptr);

KAR_ARRAY_HEADER(vartree_function_params_args, KarVartreeFunctionParams, KarVartree)

KarString* kar_vartree_create_full_function_name(const KarString* name, KarVartree** args, size_t args_count);

#endif // KAR_VARTREE_FUNCTION_PARAMS_H
