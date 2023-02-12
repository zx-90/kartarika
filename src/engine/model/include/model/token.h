/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_TOKEN_H
#define KAR_TOKEN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "core/array.h"
#include "core/cursor.h"
#include "token_type.h"

typedef struct {
	KarTokenType type;
	KarCursor cursor;
	char* str;
	KarArray children;
} KarToken;

#define kar_token_child(token, num) ((KarToken*)(token)->children.items[(num)])

KarToken* kar_token_create();
KarToken* kar_token_create_fill(KarTokenType type, KarCursor cursor, const char* str);
void kar_token_free(KarToken* token);

bool kar_token_check_type(const KarToken* token, const KarTokenType type);
bool kar_token_check_type_name(const KarToken* token, const KarTokenType type, const char* string);

void kar_token_set_str(KarToken* token, const char* str);
void kar_token_add_str(KarToken* token, const char* str);

KAR_TREE_HEADER(token_child, KarToken, KarToken);

bool kar_token_child_foreach_bool(KarToken* token, bool(*func)(KarToken* array));

size_t kar_token_child_find(KarToken* token, const KarTokenType type);

KarToken* kar_token_join_children(KarToken* token, size_t first, size_t count);
KarToken* kar_token_get_first_grandchild(KarToken* token);

void kar_token_print(const KarToken* token, FILE* stream);
char* kar_token_create_print(const KarToken* token);

#endif // KAR_TOKEN_H
