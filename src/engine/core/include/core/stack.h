/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_STACK_H
#define KAR_STACK_H

#include <stdlib.h>

typedef struct {
	size_t* stack;
	size_t depth;
} KarStack;

KarStack* kar_stack_create(size_t size);
void kar_stack_free(KarStack* stack);

void kar_stack_push(KarStack* stack, size_t value);
size_t kar_stack_pop(KarStack* stack);
size_t kar_stack_is_empty(KarStack* stack);

#endif // KAR_STACK_H
