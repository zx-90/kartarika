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
	STATIC = 0,
	DYNAMIC = 1,

	PRIVATE = 0 << 1,
	PROTECTED = 1 << 1,
	PUBLIC = 2 << 1,

	FINALIZED = 0 << 3,
	HERITABLE = 1 << 3,

	OVERLOADED = 1 << 4
} KarFunctionModificator;

typedef struct {
	uint8_t modificators;
    KarString* issueName;
	KarVartree* returnType;
} KarVartreeFunctionParams;

KarVartreeFunctionParams* kar_vartree_function_create(uint8_t modificators, const KarString* issueName, KarVartree* return_type);
void kar_vartree_function_free(void* ptr);

bool kar_vartree_function_is_static(uint8_t modificators);
bool kar_vartree_function_is_dynamic(uint8_t modificators);
bool kar_vartree_function_is_private(uint8_t modificators);
bool kar_vartree_function_is_protected(uint8_t modificators);
bool kar_vartree_function_is_public(uint8_t modificators);
bool kar_vartree_function_is_finalized(uint8_t modificators);
bool kar_vartree_function_is_heritable(uint8_t modificators);
bool kar_vartree_function_is_overloaded(uint8_t modificators);

#endif // KAR_VARTREE_FUNCTION_PARAMS_H
