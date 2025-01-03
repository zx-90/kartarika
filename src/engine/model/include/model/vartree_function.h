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

typedef enum {
	FUNC_STATIC = 0,
	FUNC_DYNAMIC = 1,

	FUNC_PRIVATE = 0 << 1,
	FUNC_PROTECTED = 1 << 1,
	FUNC_PUBLIC = 2 << 1,

	FUNC_FINALIZED = 0 << 3,
	FUNC_HERITABLE = 1 << 3,

	FUNC_OVERLOADED = 1 << 4
} KarFunctionModificator;

typedef struct {
	uint8_t modificators;
    KarString* issueName;
	KarVartree* returnType;
} KarVartreeFunction;

// TODO: Необходимо разделить части относящиеся к анализатору и к генератору.
//       libName (и соответствующее ему issueName) относится к генератору,
//       надо перенести туда.
KarVartree* kar_vartree_create_function(KarVartree* parent, const KarString* name, uint8_t modificators, const KarString* libName, KarVartree** args, size_t args_count, KarVartree* return_type);

KarVartreeFunction* kar_vartree_get_function_params(KarVartree* vartree);

bool kar_vartree_function_is_static(uint8_t modificators);
bool kar_vartree_function_is_dynamic(uint8_t modificators);
bool kar_vartree_function_is_private(uint8_t modificators);
bool kar_vartree_function_is_protected(uint8_t modificators);
bool kar_vartree_function_is_public(uint8_t modificators);
bool kar_vartree_function_is_finalized(uint8_t modificators);
bool kar_vartree_function_is_heritable(uint8_t modificators);
bool kar_vartree_function_is_overloaded(uint8_t modificators);

#endif // KAR_VARTREE_FUNCTION_PARAMS_H
