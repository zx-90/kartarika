/* Copyright © 2020,2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_ARRAY_H
#define KAR_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

#include "alloc.h"

// ----------------------------------------------------------------------------
// Структура.
// ----------------------------------------------------------------------------

#define KAR_ARRAY_STRUCT(child_type) \
	struct {                         \
		size_t count;                \
		size_t capacity;             \
		child_type** items;          \
	}

// ----------------------------------------------------------------------------
// Заголовочный файл.
// ----------------------------------------------------------------------------

#define KAR_ARRAY_HEADER_INIT(prefix, parent_type)                    \
	void kar_##prefix##_init(parent_type* parent);
#define KAR_ARRAY_HEADER_CLEAR(prefix, parent_type)                   \
	void kar_##prefix##_clear(parent_type* parent);
#define KAR_ARRAY_HEADER_COUNT(prefix, parent_type)                   \
	size_t kar_##prefix##_count(const parent_type* parent);
#define KAR_ARRAY_HEADER_EMPTY(prefix, parent_type)                   \
	bool kar_##prefix##_empty(const parent_type* parent);
#define KAR_ARRAY_HEADER_CAPACITY(prefix, parent_type)                \
	size_t kar_##prefix##_capacity(const parent_type* parent);
#define KAR_ARRAY_HEADER_GET(prefix, parent_type, child_type)         \
	child_type* kar_##prefix##_get(const parent_type* parent, size_t num);
#define KAR_ARRAY_HEADER_GET_LAST(prefix, parent_type, child_type)    \
	child_type* kar_##prefix##_get_last(const parent_type* parent, size_t num);
#define KAR_ARRAY_HEADER_ADD(prefix, parent_type, child_type)         \
	void kar_##prefix##_add(parent_type* parent, child_type* child);
#define KAR_ARRAY_HEADER_INSERT(prefix, parent_type, child_type)      \
	void kar_##prefix##_insert(parent_type* parent, child_type* child, size_t num);
#define KAR_ARRAY_HEADER_TEAR(prefix, parent_type, child_type)        \
	child_type* kar_##prefix##_tear(parent_type* parent, size_t num);
#define KAR_ARRAY_HEADER_ERASE(prefix, parent_type)                   \
	void kar_##prefix##_erase(parent_type* parent, size_t num);
#define KAR_ARRAY_HEADER_MOVE_TO_END(prefix, parent_type)             \
    void kar_##prefix##_move_to_end(parent_type* from, parent_type* to, size_t begin, size_t count);

#define KAR_ARRAY_HEADER(prefix, parent_type, child_type)      \
	KAR_ARRAY_HEADER_INIT(prefix, parent_type)                 \
	KAR_ARRAY_HEADER_CLEAR(prefix, parent_type)                \
	KAR_ARRAY_HEADER_COUNT(prefix, parent_type)                \
	KAR_ARRAY_HEADER_EMPTY(prefix, parent_type)                \
	KAR_ARRAY_HEADER_CAPACITY(prefix, parent_type)             \
	KAR_ARRAY_HEADER_GET(prefix, parent_type, child_type)      \
	KAR_ARRAY_HEADER_GET_LAST(prefix, parent_type, child_type) \
    KAR_ARRAY_HEADER_ADD(prefix, parent_type, child_type)      \
    KAR_ARRAY_HEADER_INSERT(prefix, parent_type, child_type)   \
    KAR_ARRAY_HEADER_TEAR(prefix, parent_type, child_type)     \
    KAR_ARRAY_HEADER_ERASE(prefix, parent_type)                \
    KAR_ARRAY_HEADER_MOVE_TO_END(prefix, parent_type)

// ----------------------------------------------------------------------------
// Файл с кодом.
// ----------------------------------------------------------------------------

#define KAR_ARRAY_CODE_INIT(prefix, parent_type, field) \
	void kar_##prefix##_init(parent_type* parent) {     \
		parent->field.count = 0;                        \
		parent->field.capacity = 0;                     \
		parent->field.items = NULL;                     \
	}

#define KAR_ARRAY_CODE_CLEAR(prefix, parent_type, field, fn)  \
	void kar_##prefix##_clear(parent_type* parent) {          \
		if (fn != NULL) {                                     \
			while(parent->field.count--) {                    \
				fn(parent->field.items[parent->field.count]); \
			}                                                 \
		}                                                     \
		if (parent->field.items) {                            \
			KAR_FREE(parent->field.items);                    \
		}                                                     \
		kar_##prefix##_init(parent);                          \
	}

#define KAR_ARRAY_CODE_COUNT(prefix, parent_type, field)     \
	size_t kar_##prefix##_count(const parent_type* parent) { \
		return parent->field.count;                          \
	}

#define KAR_ARRAY_CODE_EMPTY(prefix, parent_type, field)   \
	bool kar_##prefix##_empty(const parent_type* parent) { \
		return parent->field.count == 0;                   \
	}

#define KAR_ARRAY_CODE_CAPACITY(prefix, parent_type, field)     \
	size_t kar_##prefix##_capacity(const parent_type* parent) { \
		return parent->field.capacity;                          \
	}

#define KAR_ARRAY_CODE_GET(prefix, parent_type, child_type, field)                \
	child_type* kar_##prefix##_get(const parent_type* parent, const size_t num) { \
		if (num >= parent->field.count) {                                         \
			return NULL;                                                          \
		}                                                                         \
		return parent->field.items[num];                                          \
	}

#define KAR_ARRAY_CODE_GET_LAST(prefix, parent_type, child_type, field)                \
	child_type* kar_##prefix##_get_last(const parent_type* parent, const size_t num) { \
		if (num >= parent->field.count) {                                              \
			return NULL;                                                               \
		}                                                                              \
		return parent->field.items[parent->field.count - num -   1];                   \
	}

#define KAR_ARRAY_CODE_WIDE_CAPACITY(prefix, parent_type, child_type, field)   \
	static void kar_##prefix##_wide_capacity(parent_type* parent) {            \
		if (!parent->field.capacity) {                                         \
			parent->field.capacity = 1;                                        \
		} else {                                                               \
			parent->field.capacity *= 2;                                       \
		}                                                                      \
		KAR_REALLOC(parent->field.items, child_type*, parent->field.capacity); \
	}


#define KAR_ARRAY_CODE_ADD(prefix, parent_type, child_type, field)    \
	void kar_##prefix##_add(parent_type* parent, child_type* child) { \
		if (parent->field.count == parent->field.capacity) {          \
			kar_##prefix##_wide_capacity(parent);                     \
		}                                                             \
		parent->field.items[parent->field.count] = child;             \
		parent->field.count++;                                        \
	}

#define KAR_ARRAY_CODE_INSERT(prefix, parent_type, child_type, field)                \
	void kar_##prefix##_insert(parent_type* parent, child_type* child, size_t num) { \
		if (num >= parent->field.count) {                                            \
			kar_##prefix##_add(parent, child);                                       \
			return;                                                                  \
		}                                                                            \
		if (parent->field.count == parent->field.capacity) {                         \
			kar_##prefix##_wide_capacity(parent);                                    \
		}                                                                            \
		for (size_t i = parent->field.count; i > num; --i) {                         \
			parent->field.items[i] = parent->field.items[i - 1];                     \
		}                                                                            \
		parent->field.items[num] = child;                                            \
		parent->field.count++;                                                       \
	}

#define KAR_ARRAY_CODE_TEAR(prefix, parent_type, child_type, field)    \
	child_type* kar_##prefix##_tear(parent_type* parent, size_t num) { \
		if (num >= parent->field.count) {                              \
			return NULL;                                               \
		}                                                              \
		void* teared = parent->field.items[num];                       \
		parent->field.count--;                                         \
		size_t n;                                                      \
		for (n = num; n < parent->field.count; ++n) {                  \
			parent->field.items[n] = parent->field.items[n + 1];       \
		}                                                              \
		return teared;                                                 \
	}

#define KAR_ARRAY_CODE_ERASE(prefix, parent_type, child_type, fn) \
	void kar_##prefix##_erase(parent_type* parent, size_t num) {  \
		child_type* teared = kar_##prefix##_tear(parent, num);    \
		if (teared && fn != NULL) {                               \
			fn(teared);                                           \
		}                                                         \
	}

#define KAR_ARRAY_CODE_MOVE_TO_END(prefix, parent_type, field)                                        \
	void kar_##prefix##_move_to_end(parent_type* from, parent_type* to, size_t begin, size_t count) { \
		while (to->field.capacity < to->field.count + count) {                                        \
			kar_##prefix##_wide_capacity(to);                                                         \
		}                                                                                             \
		for (size_t i = 0; i < count; ++i) {                                                          \
			to->field.items[to->field.count + i] = from->field.items[begin + i];                      \
		}                                                                                             \
		for (size_t i = begin; i < from->field.count - count; ++i) {                                  \
			from->field.items[i] = from->field.items[i + count];                                      \
		}                                                                                             \
		from->field.count -= count;                                                                   \
		to->field.count += count;                                                                     \
	}

#define KAR_ARRAY_CODE(prefix, parent_type, child_type, field, fn)       \
	KAR_ARRAY_CODE_INIT(prefix, parent_type, field)                      \
	KAR_ARRAY_CODE_CLEAR(prefix, parent_type, field, fn)                 \
	KAR_ARRAY_CODE_COUNT(prefix, parent_type, field)                     \
	KAR_ARRAY_CODE_EMPTY(prefix, parent_type, field)                     \
	KAR_ARRAY_CODE_CAPACITY(prefix, parent_type, field)                  \
	KAR_ARRAY_CODE_GET(prefix, parent_type, child_type, field)           \
	KAR_ARRAY_CODE_GET_LAST(prefix, parent_type, child_type, field)      \
	KAR_ARRAY_CODE_WIDE_CAPACITY(prefix, parent_type, child_type, field) \
	KAR_ARRAY_CODE_ADD(prefix, parent_type, child_type, field)           \
	KAR_ARRAY_CODE_INSERT(prefix, parent_type, child_type, field)        \
	KAR_ARRAY_CODE_TEAR(prefix, parent_type, child_type, field)          \
	KAR_ARRAY_CODE_ERASE(prefix, parent_type, child_type, fn)            \
	KAR_ARRAY_CODE_MOVE_TO_END(prefix, parent_type, field)

#endif // KAR_ARRAY_H
