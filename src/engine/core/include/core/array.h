/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_ARRAY_H
#define KAR_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
	size_t count;
	size_t capacity;
	void** items;
} KarArray;

typedef void(KarArrayFreeFn)(void* item);

void kar_array_init(KarArray* array);
void kar_array_clear(KarArray* array, KarArrayFreeFn* fn);

void kar_array_add(KarArray* array, void* child);
void kar_array_insert(KarArray* array, void* child, size_t num);
void* kar_array_tear(KarArray* array, size_t num);
void kar_array_erase(KarArray* array, size_t num, KarArrayFreeFn* fn);
void kar_array_move_to_end(KarArray* from, KarArray* to, size_t begin, size_t count);

#define KAR_TREE_HEADER(prefix, parent_type, child_type)                                        \
void kar_##prefix##_add(parent_type* array, child_type* child);                            \
void kar_##prefix##_insert(parent_type* array, child_type* child, size_t num);             \
child_type* kar_##prefix##_tear(parent_type* array, size_t num);                           \
void kar_##prefix##_erase(parent_type* array, size_t num);                           \
void kar_##prefix##_move_to_end(parent_type* from, parent_type* to, size_t begin, size_t count)

/*; \
bool kar_##prefix##_foreach_bool(child_type* array, bool(*fn)(child_type* array))*/

#define KAR_TREE_CODE(prefix, parent_type, child_type, field, fn) \
void kar_##prefix##_add(parent_type* array, child_type* child) { kar_array_add(&array->field, child); } \
void kar_##prefix##_insert(parent_type* array, child_type* child, size_t num) { kar_array_insert(&array->field, child, num); } \
child_type* kar_##prefix##_tear(parent_type* array, size_t num) { return (child_type*)kar_array_tear(&array->field, num); } \
void kar_##prefix##_erase(parent_type* array, size_t num) { kar_array_erase(&array->field, num, (KarArrayFreeFn*)(fn)); } \
void kar_##prefix##_move_to_end(parent_type* from, parent_type* to, size_t begin, size_t count) { kar_array_move_to_end(&from->field, &to->field, begin, count); }

/* \
bool kar_##prefix##_foreach_bool(child_type* array, bool(*func)(child_type* array)) \
    { for (size_t i = 0; i < array->field.count; i++) { if (!kar_##prefix##_foreach_bool(array->field.items[i], func)) return false; } return func(array); } \ */


#endif // KAR_ARRAY_H
