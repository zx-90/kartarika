/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_TREE_SET_H
#define KAR_TREE_SET_H

#include <stddef.h>
#include <stdbool.h>

#include "alloc.h"

// ----------------------------------------------------------------------------
// Структура.
// ----------------------------------------------------------------------------

#define KAR_TREE_SET_STRUCT(type) \
	struct {                      \
	    type* parent;             \
	    size_t count;             \
	    size_t capacity;          \
	    type** items;             \
	}

// ----------------------------------------------------------------------------
// Заголовочный файл.
// ----------------------------------------------------------------------------

#define KAR_TREE_SET_HEADER_INIT(prefix, type)                    \
	void kar_##prefix##_init(type* parent);
#define KAR_TREE_SET_HEADER_CLEAR(prefix, type)                   \
	void kar_##prefix##_clear(type* parent);
#define KAR_TREE_SET_HEADER_PARENT(prefix, type)                  \
	type* kar_##prefix##_parent(const type* parent);
#define KAR_TREE_SET_HEADER_COUNT(prefix, type)                   \
	size_t kar_##prefix##_count(const type* parent);
#define KAR_TREE_SET_HEADER_EMPTY(prefix, type)                   \
	bool kar_##prefix##_empty(const type* parent);
#define KAR_TREE_SET_HEADER_CAPACITY(prefix, type)                \
	size_t kar_##prefix##_capacity(const type* parent);
#define KAR_TREE_SET_HEADER_GET(prefix, type)                     \
	type* kar_##prefix##_get(const type* parent, size_t num);
#define KAR_TREE_SET_HEADER_GET_LAST(prefix, type)                \
	type* kar_##prefix##_get_last(const type* parent, size_t num);
#define KAR_TREE_SET_HEADER_ADD(prefix, type)                     \
	size_t kar_##prefix##_add(type* parent, type* child);
#define KAR_TREE_SET_HEADER_TEAR(prefix, type)                    \
	type* kar_##prefix##_tear(type* parent, size_t num);
#define KAR_TREE_SET_HEADER_ERASE(prefix, type)                   \
	void kar_##prefix##_erase(type* parent, size_t num);

#define KAR_TREE_SET_HEADER(prefix, type)      \
	KAR_TREE_SET_HEADER_INIT(prefix, type)     \
	KAR_TREE_SET_HEADER_CLEAR(prefix, type)    \
	KAR_TREE_SET_HEADER_PARENT(prefix, type)   \
	KAR_TREE_SET_HEADER_COUNT(prefix, type)    \
	KAR_TREE_SET_HEADER_EMPTY(prefix, type)    \
	KAR_TREE_SET_HEADER_CAPACITY(prefix, type) \
	KAR_TREE_SET_HEADER_GET(prefix, type)      \
	KAR_TREE_SET_HEADER_GET_LAST(prefix, type) \
	KAR_TREE_SET_HEADER_ADD(prefix, type)      \
	KAR_TREE_SET_HEADER_TEAR(prefix, type)     \
	KAR_TREE_SET_HEADER_ERASE(prefix, type)    \

// ----------------------------------------------------------------------------
// Файл с кодом.
// ----------------------------------------------------------------------------

#define KAR_TREE_SET_CODE_INIT(prefix, type, field) \
	void kar_##prefix##_init(type* parent) {        \
	    parent->field.parent = NULL;                \
	    parent->field.count = 0;                    \
	    parent->field.capacity = 0;                 \
	    parent->field.items = NULL;                 \
	}

#define KAR_TREE_SET_CODE_CLEAR(prefix, type, field, free_fn)      \
	void kar_##prefix##_clear(type* parent) {                      \
		if (free_fn != NULL) {                                     \
			while(parent->field.count--) {                         \
				free_fn(parent->field.items[parent->field.count]); \
			}                                                      \
		}                                                          \
		if (parent->field.items) {                                 \
			KAR_FREE(parent->field.items);                         \
		}                                                          \
		kar_##prefix##_init(parent);                               \
	}

#define KAR_TREE_SET_CODE_PARENT(prefix, type, field) \
	type* kar_##prefix##_parent(const type* parent) { \
	    return (type*)parent->field.parent;           \
    }

#define KAR_TREE_SET_CODE_COUNT(prefix, type, field)  \
	size_t kar_##prefix##_count(const type* parent) { \
	    return parent->field.count;                   \
	}

#define KAR_TREE_SET_CODE_EMPTY(prefix, type, field) \
	bool kar_##prefix##_empty(const type* parent) {  \
	    return parent->field.count == 0;             \
	}

#define KAR_TREE_SET_CODE_CAPACITY(prefix, type, field)  \
	size_t kar_##prefix##_capacity(const type* parent) { \
	    return parent->field.capacity;                   \
	}

#define KAR_TREE_SET_CODE_GET(prefix, type, field)                   \
	type* kar_##prefix##_get(const type* parent, const size_t num) { \
	    if (num >= parent->field.count) {                            \
	        return NULL;                                             \
	    }                                                            \
	    return parent->field.items[num];                             \
	}

#define KAR_TREE_SET_CODE_GET_LAST(prefix, type, field)                   \
	type* kar_##prefix##_get_last(const type* parent, const size_t num) { \
	    if (num >= parent->field.count) {                                 \
	        return NULL;                                                  \
	    }                                                                 \
	    return parent->field.items[parent->field.count - num -   1];      \
	}

#define KAR_TREE_SET_CODE_WIDE_CAPACITY(prefix, type, field)             \
	static void kar_##prefix##_wide_capacity(type* parent) {             \
	    if (!parent->field.capacity) {                                   \
	        parent->field.capacity = 1;                                  \
	    } else {                                                         \
	        parent->field.capacity *= 2;                                 \
	    }                                                                \
	    KAR_REALLOC(parent->field.items, type*, parent->field.capacity); \
	}


#define KAR_TREE_SET_CODE_ADD(prefix, type, field, less_fn, equal_fn) \
	size_t kar_##prefix##_add(type* parent, type* added) {            \
	    if (parent->field.count == parent->field.capacity) {          \
	        kar_##prefix##_wide_capacity(parent);                     \
	    }                                                             \
	                                                                  \
	    size_t num = 0;                                               \
	    size_t range = parent->field.count;                           \
	    while (range > 1) {                                           \
	        range = (range + 1) / 2;                                  \
	        type* child = parent->field.items[num];                   \
	        if (less_fn(child, added)) {                              \
	            if (num + range < parent->field.count) {              \
	                num += range;                                     \
	            }                                                     \
	        } else {                                                  \
	            if (num >= range) {                                   \
	                num -= range;                                     \
	            }                                                     \
	        }                                                         \
	    }                                                             \
	    if (num < parent->field.count &&                              \
	        less_fn(added, parent->field.items[num])) {               \
	        num++;                                                    \
	    }                                                             \
	    if (num < parent->field.count &&                              \
	        equal_fn(added, parent->field.items[num])) {              \
	        return (size_t)-1;                                        \
	    }                                                             \
	                                                                  \
	    for (size_t i = parent->field.count; i > num; --i) {          \
	        parent->field.items[i] = parent->field.items[i - 1];      \
	    }                                                             \
	                                                                  \
	    parent->field.items[num] = added;                             \
	    added->field.parent = parent;                                 \
	    parent->field.count++;                                        \
	    return num;                                                   \
	}

#define KAR_TREE_SET_CODE_TEAR(prefix, type, field)              \
	type* kar_##prefix##_tear(type* parent, size_t num) {        \
	    if (num >= parent->field.count) {                        \
	        return NULL;                                         \
	    }                                                        \
	    type* teared = parent->field.items[num];                 \
	    teared->field.parent = NULL;                             \
	    parent->field.count--;                                   \
	    size_t n;                                                \
	    for (n = num; n < parent->field.count; ++n) {            \
	        parent->field.items[n] = parent->field.items[n + 1]; \
	    }                                                        \
	    return teared;                                           \
	}

#define KAR_TREE_SET_CODE_ERASE(prefix, type, free_fn)    \
	void kar_##prefix##_erase(type* parent, size_t num) { \
	    type* teared = kar_##prefix##_tear(parent, num);  \
	    if (teared && free_fn != NULL) {                  \
	        free_fn(teared);                              \
	    }                                                 \
	}

#define KAR_TREE_SET_CODE(prefix, type, field, less_fn, equal_fn, free_fn) \
	    KAR_TREE_SET_CODE_INIT(prefix, type, field)                        \
	    KAR_TREE_SET_CODE_CLEAR(prefix, type, field, free_fn)              \
	KAR_TREE_SET_CODE_PARENT(prefix, type, field)                          \
	    KAR_TREE_SET_CODE_COUNT(prefix, type, field)                       \
	    KAR_TREE_SET_CODE_EMPTY(prefix, type, field)                       \
	    KAR_TREE_SET_CODE_CAPACITY(prefix, type, field)                    \
	    KAR_TREE_SET_CODE_GET(prefix, type, field)                         \
	    KAR_TREE_SET_CODE_GET_LAST(prefix, type, field)                    \
	    KAR_TREE_SET_CODE_WIDE_CAPACITY(prefix, type, field)               \
	    KAR_TREE_SET_CODE_ADD(prefix, type, field, less_fn, equal_fn)      \
	    KAR_TREE_SET_CODE_TEAR(prefix, type, field)                        \
	    KAR_TREE_SET_CODE_ERASE(prefix, type, free_fn)

#endif // KAR_TREE_SET_H
