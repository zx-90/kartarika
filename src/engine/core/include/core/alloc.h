/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h>
#include <stdio.h>

// #define K_DEBUG

#ifdef K_DEBUG

#define K_CREATE(var, type)\
    type* var = (type*)malloc(sizeof(type)); \
    fprintf( stderr, "ALLOC: %p, TYPE: %s,  AT: %s:%d\n", (void*)(var), #type, __FILE__, __LINE__);

#define K_CREATES(var, type, count)\
    type* var = (type*)malloc(sizeof(type) * (count)); \
    fprintf( stderr, "ALLOC: %p, TYPE: %s x %ld,  AT: %s:%d\n", (void*)(var), #type, count, __FILE__, __LINE__);

#define K_ALLOC(var, type)\
    var = (type*)malloc(sizeof(type)); \
    fprintf( stderr, "ALLOC: %p, TYPE: %s,  AT: %s:%d\n", (void*)(var), #type, __FILE__, __LINE__);

#define K_ALLOCS(var, type, count)\
    var = (type*)malloc(sizeof(type) * (count)); \
    fprintf( stderr, "ALLOC: %p, TYPE: %s x %ld,  AT: %s:%d\n", (void*)(var), #type, count, __FILE__, __LINE__);

#define K_FREE(var) (fprintf(stderr, "FREE : %p\n", (void*)(var)), free(var))

#else

#define K_CREATE(var, type) type* var = (type*)malloc(sizeof(type));

#define K_CREATES(var, type, count) type* var = ((type*)malloc(sizeof(type) * count));

#define K_ALLOC(var, type) var = (type*)malloc(sizeof(type));

#define K_ALLOCS(var, type, count) var = ((type*)malloc(sizeof(type) * count));

#define K_FREE(var) free(var);

#endif // K_DEBUG_ON

#endif // ALLOC_H
