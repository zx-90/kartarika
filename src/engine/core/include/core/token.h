/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_TOKEN_H
#define KAR_TOKEN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "array.h"
#include "cursor.h"
#include "token_type.h"

typedef struct KarStructToken {
	KarTokenType type;
	KarCursor cursor;
	char* str;
	KarArray children;
} KarToken;

#define kar_token_child(token, num) ((KarToken*)(token)->children.items[(num)])

KarToken* kar_token_create();
void kar_token_free(KarToken* token);

bool kar_token_check_type(const KarToken* token, const KarTokenType type);
bool kar_token_check_type_name(const KarToken* token, const KarTokenType type, const char* string);

void kar_token_set_str(KarToken* token, const char* str);
void kar_token_add_str(KarToken* token, const char* str);

KAR_ARRAY_HEADER(token_child, KarToken);

void kar_token_print(const KarToken* token, FILE* stream);
void kar_token_print_level(const KarToken* token, FILE* stream, size_t level);

char* kar_token_sprint(const KarToken* token);
char* kar_token_sprint_level(const KarToken* token, char* buffer, size_t level);

#endif // KAR_TOKEN_H
