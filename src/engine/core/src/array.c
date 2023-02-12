/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/array.h"

#include <string.h>

#include "core/alloc.h"

void kar_array_init(KarArray* array) {
	array->count = 0;
	array->capacity = 0;
	array->items = NULL;
}

void kar_array_clear(KarArray* array, KarArrayFreeFn* fn) {
	if (fn != NULL) {
		while(array->count--) {
			fn(array->items[array->count]);
		}
	}
	if (array->items) {
		KAR_FREE(array->items);
	}
	kar_array_init(array);
}

static void kar_array_wide_capacity(KarArray* array) {
	if (!array->capacity) {
		array->capacity = 1;
	} else {
		array->capacity *= 2;
	}
	KAR_REALLOC(array->items, void*, array->capacity);
}

void kar_array_add(KarArray* array, void* child) {
	if (array->count == array->capacity) {
		kar_array_wide_capacity(array);
	}
	array->items[array->count] = child;
	array->count++;
}

void kar_array_insert(KarArray* array, void* child, size_t num) {
	if (num >= array->count) {
		kar_array_add(array, child);
		return;
	}
	if (array->count == array->capacity) {
		kar_array_wide_capacity(array);
	}
	for (size_t i = array->count; i > num; --i) {
		array->items[i] = array->items[i - 1];
	}
	array->items[num] = child;
	array->count++;
}

void* kar_array_tear(KarArray* array, size_t num) {
	if (num >= array->count) {
		return NULL;
	}
	void* teared = array->items[num];
	array->count--;
	size_t n;
	for (n = num; n < array->count; ++n) {
		array->items[n] = array->items[n + 1];
	}
	return teared;
}

void kar_array_erase(KarArray* array, size_t num, KarArrayFreeFn* fn) {
	void* teared = kar_array_tear(array, num);
	if (teared && fn!= NULL) {
		fn(teared);
	}
}

void kar_array_move_to_end(KarArray* from, KarArray* to, size_t begin, size_t count) {
	while (to->capacity < to->count + count) {
		kar_array_wide_capacity(to);
	}
	
	for (size_t i = 0; i < count; ++i) {
		to->items[to->count + i] = from->items[begin + i];
	}
	
	for (size_t i = begin; i < from->count - count; ++i) {
		from->items[i] = from->items[i + count];
	}
	
	from->count -= count;
	to->count += count;
}
