/* Copyright © 2024 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_LOCAL_VARS_H
#define KAR_LOCAL_VARS_H

#include "core/array.h"
#include "core/set.h"

#include "vartree.h"

//-----------------------------------------------------------------------------

typedef struct {
	KarString* name;
	KarVartree* type;
	void* value;
} KarLocalVar;

KarLocalVar* kar_local_var_create(KarString* name, KarVartree* type, void* value);
void kar_local_var_free(KarLocalVar* var);

bool kar_local_var_less(KarLocalVar* var1, KarLocalVar* var2);
bool kar_local_var_equal(KarLocalVar* var1, KarLocalVar* var2);

//-----------------------------------------------------------------------------

typedef struct {
	KAR_SET_STRUCT(KarLocalVar) vars;
} KarLocalBlock;

KarLocalBlock* kar_local_block_create();
void kar_local_block_free(KarLocalBlock* block);

KarLocalVar* kar_local_block_get_var_by_name(KarLocalBlock* block, KarString *name);

KAR_SET_HEADER(local_block_var, KarLocalBlock, KarLocalVar)

//-----------------------------------------------------------------------------

// TODO: По-хорошему надо как список реализовать, а не динамический массив.

typedef struct {
	KAR_ARRAY_STRUCT(KarLocalBlock) blocks;
} KarLocalStack;

KarLocalStack* kar_local_stack_create();
void kar_local_stack_free(KarLocalStack* stack);

KAR_ARRAY_HEADER(local_stack_block, KarLocalStack, KarLocalBlock)

//-----------------------------------------------------------------------------

#endif // KAR_LOCAL_VARS_H
