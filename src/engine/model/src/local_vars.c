/* Copyright © 2024 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/local_vars.h"

//-----------------------------------------------------------------------------

KarLocalVar* kar_local_var_create(KarString* name, KarVartree* type, void* value) {
	KAR_CREATE(var, KarLocalVar);

	var->name = kar_string_create(name);
	var->type = type;
	var->value = value;

	return var;
}

void kar_local_var_free(KarLocalVar* var) {

	KAR_FREE(var->name);
	KAR_FREE(var);
}

bool kar_local_var_less(KarLocalVar* var1, KarLocalVar* var2) {
	if (var1->name == NULL || var2->name == NULL) {
		return false;
	}
	return kar_string_less(var1->name, var2->name);
}

bool kar_local_var_equal(KarLocalVar* var1, KarLocalVar* var2) {
	if (var1->name == NULL || var2->name == NULL) {
		return false;
	}
	return kar_string_equal(var1->name, var2->name);
}

//-----------------------------------------------------------------------------

KarLocalBlock* kar_local_block_create() {
	KAR_CREATE(block, KarLocalBlock);

	kar_local_block_var_init(block);

	return block;
}

void kar_local_block_free(KarLocalBlock* block) {
	kar_local_block_var_clear(block);
	KAR_FREE(block);
}

KarLocalVar* kar_local_block_get_var_by_name(KarLocalBlock* block, KarString* name) {
	for (size_t i = 0; i < kar_local_block_var_count(block); i++) {
		KarLocalVar* var = kar_local_block_var_get(block, i);
		if (kar_string_equal(var->name, name)) {
			return var;
		}
	}
	return NULL;
}

KAR_SET_CODE(local_block_var, KarLocalBlock, KarLocalVar, vars, kar_local_var_less, kar_local_var_equal, kar_local_var_free)

//-----------------------------------------------------------------------------

KarLocalStack* kar_local_stack_create() {
	KAR_CREATE(stack, KarLocalStack);

	kar_local_stack_block_init(stack);

	return stack;
}

void kar_local_stack_free(KarLocalStack* stack) {
	kar_local_stack_block_clear(stack);
	KAR_FREE(stack);
}

KAR_ARRAY_CODE(local_stack_block, KarLocalStack, KarLocalBlock, blocks, kar_local_block_free)

//-----------------------------------------------------------------------------

