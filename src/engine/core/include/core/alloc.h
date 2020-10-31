/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_ALLOC_H
#define KAR_ALLOC_H

#include <stdlib.h>
#include <stdio.h>

#define KAR_ALLOC_DEBUG

#ifdef KAR_ALLOC_DEBUG

#define KAR_CREATE(var, type)\
    type* (var) = (type*)malloc(sizeof(type)); if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); } \
    fprintf(stderr, "ALLOC: %p, TYPE: %s,  AT: %s:%d\n", (void*)(var), #type, __FILE__, __LINE__);

#define KAR_CREATES(var, type, count)\
    type* (var) = (type*)malloc(sizeof(type) * (count)); if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); } \
    fprintf(stderr, "ALLOC: %p, TYPE: %s x %ld,  AT: %s:%d\n", (void*)(var), #type, (count), __FILE__, __LINE__);

#define KAR_ALLOC(var, type)\
    (var) = (type*)malloc(sizeof(type)); if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); } \
    fprintf(stderr, "ALLOC: %p, TYPE: %s,  AT: %s:%d\n", (void*)(var), #type, __FILE__, __LINE__);

#define KAR_ALLOCS(var, type, count)\
    (var) = (type*)malloc(sizeof(type) * (count)); if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); } \
    fprintf(stderr, "ALLOC: %p, TYPE: %s x %ld,  AT: %s:%d\n", (void*)(var), #type, (count), __FILE__, __LINE__);

#define KAR_REALLOC(var, type, count)\
	fprintf(stderr, "REALLOC:\nFREE : %p  AT: %s:%d\n", (void*)(var), __FILE__, __LINE__); \
    (var) = (type*)realloc((var), sizeof(type) * (count)); if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); } \
    fprintf(stderr, "ALLOC: %p, TYPE: %s x %ld,  AT: %s:%d\n", (void*)(var), #type, (count), __FILE__, __LINE__);

#define KAR_FREE(var) (fprintf(stderr, "FREE : %p,  AT: %s:%d\n", (void*)(var), __FILE__, __LINE__), fflush(stderr), free(var))

#else

#define KAR_CREATE(var, type) type* (var) = (type*)malloc(sizeof(type));  if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); }

#define KAR_CREATES(var, type, count) type* (var) = ((type*)malloc(sizeof(type) * (count)));  if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); }

#define KAR_ALLOC(var, type) (var) = (type*)malloc(sizeof(type)); if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); }

#define KAR_ALLOCS(var, type, count) (var) = ((type*)malloc(sizeof(type) * (count))); if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); }

#define KAR_REALLOC(var, type, count) (var) = (type*)realloc((var), sizeof(type) * (count)); if (!(var)) { fprintf(stderr, "Ошибка выделения памяти."); exit(1); }

#define KAR_FREE(var) free(var);

#endif // KAR_DEBUG_ON

#endif // KAR_ALLOC_H
