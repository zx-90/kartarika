/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/stack.h"

#include "core/alloc.h"

KarStack* kar_stack_create(size_t size) {
	KAR_CREATE(stack, KarStack);
	
	KAR_ALLOCS(stack->stack, size_t, size);
	stack->depth = 0;
	
	return stack;
}

void kar_stack_free(KarStack* stack) {
	KAR_FREE(stack->stack);
	KAR_FREE(stack);
}

void kar_stack_push(KarStack* stack, size_t value) {
	stack->stack[stack->depth] = value;
	stack->depth++;
}

size_t kar_stack_pop(KarStack* stack) {
	stack->depth--;
	return stack->stack[stack->depth];
}

size_t kar_stack_is_empty(KarStack* stack) {
	return stack->depth == 0;
}

