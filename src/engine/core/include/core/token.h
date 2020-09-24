/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "cursor.h"
#include "token_type.h"

typedef struct KStructToken {
	KTokenType type;
	KCursor cursor;
	char* str;

	size_t children_count;
	size_t children_capacity;
	struct KStructToken** children;
} KToken;

#ifdef __cplusplus
extern "C" {
#endif

KToken* k_token_create();
void k_token_free(KToken* token);

bool k_token_check_type(const KToken* token, const KTokenType type);
bool k_token_check_type_name(const KToken* token, const KTokenType type, const char* string);

void k_token_set_str(KToken* token, const char* str);
void k_token_add_str(KToken* token, const char* str);

void k_token_add_child(KToken* token, KToken* child);
void k_token_insert_child(KToken* token, KToken* child, size_t num);
KToken* k_token_tear_child(KToken* token, size_t num);
void k_token_erase_child(KToken* token, size_t num);

bool k_token_foreach(KToken* token, bool(*fn)(KToken* token));

void k_token_print(const KToken* token, FILE* stream);
void k_token_print_level(const KToken* token, FILE* stream, size_t level);

char* k_token_sprint(const KToken* token);
char* k_token_sprint_level(const KToken* token, char* buffer, size_t level);

#ifdef __cplusplus
}
#endif

#endif // TOKEN_H
