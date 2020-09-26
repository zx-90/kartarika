/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_ALLOC_H
#define KAR_ALLOC_H

#include <stdlib.h>

// #define KAR_DEBUG

#ifdef KAR_DEBUG

#include <stdio.h>

#define KAR_CREATE(var, type)\
    type* var = (type*)malloc(sizeof(type)); \
    fprintf( stderr, "ALLOC: %p, TYPE: %s,  AT: %s:%d\n", (void*)(var), #type, __FILE__, __LINE__);

#define KAR_CREATES(var, type, count)\
    type* var = (type*)malloc(sizeof(type) * (count)); \
    fprintf( stderr, "ALLOC: %p, TYPE: %s x %ld,  AT: %s:%d\n", (void*)(var), #type, count, __FILE__, __LINE__);

#define KAR_ALLOC(var, type)\
    var = (type*)malloc(sizeof(type)); \
    fprintf( stderr, "ALLOC: %p, TYPE: %s,  AT: %s:%d\n", (void*)(var), #type, __FILE__, __LINE__);

#define KAR_ALLOCS(var, type, count)\
    var = (type*)malloc(sizeof(type) * (count)); \
    fprintf( stderr, "ALLOC: %p, TYPE: %s x %ld,  AT: %s:%d\n", (void*)(var), #type, count, __FILE__, __LINE__);

#define KAR_FREE(var) (fprintf(stderr, "FREE : %p\n", (void*)(var)), free(var))

#else

#define KAR_CREATE(var, type) type* var = (type*)malloc(sizeof(type));

#define KAR_CREATES(var, type, count) type* var = ((type*)malloc(sizeof(type) * count));

#define KAR_ALLOC(var, type) var = (type*)malloc(sizeof(type));

#define KAR_ALLOCS(var, type, count) var = ((type*)malloc(sizeof(type) * count));

#define KAR_FREE(var) free(var);

#endif // KAR_DEBUG_ON

#endif // KAR_ALLOC_H
