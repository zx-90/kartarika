/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/token.h"

#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "core/alloc.h"
#include "core/string.h"

KarToken* kar_token_create()
{
	KAR_CREATE(token, KarToken);
	
	token->type = KAR_TOKEN_UNKNOWN;
	kar_cursor_init(&token->cursor);
	token->str = NULL;
	kar_token_child_init(token);
	
	return token;
}

KarToken* kar_token_create_fill(KarTokenType type, KarCursor cursor, const KarString* str)
{
	KAR_CREATE(token, KarToken);
	
	token->type = type;
	token->cursor = cursor;
	if (str) {
		token->str = kar_string_create(str);
	} else {
		token->str = NULL;
	}
	kar_token_child_init(token);
	
	return token;
}

void kar_token_free(KarToken* token)
{
	if (token->str) {
		kar_string_free(token->str);
	}
	kar_token_child_clear(token);
	KAR_FREE(token);
}

bool kar_token_check_type(const KarToken* token, const KarTokenType type) {
	return token->type == type;
}

bool kar_token_check_type_name(const KarToken* token, const KarTokenType type, const KarString* string) {
	return token->type == type && kar_string_equal(token->str, string);
}

void kar_token_set_str(KarToken* token, const KarString* str) {
	if (token->str) {
		kar_string_free(token->str);
	}
	if (!str) {
		token->str = NULL;
		return;
	}
	token->str = kar_string_create(str);
}

void kar_token_add_str(KarToken* token, const KarString* str) {
	if (!token->str) {
		kar_token_set_str(token, str);
		return;
	}
	KarString* new_string = kar_string_create_concat(token->str, str);
	kar_string_free(token->str);
	token->str = new_string;
}

KAR_ARRAY_CODE(token_child, KarToken, KarToken, children, kar_token_free)

bool kar_token_child_foreach_bool(KarToken* token, bool(*func)(KarToken* array)) {
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		if (!kar_token_child_foreach_bool(kar_token_child_get(token, i), func)) {
			return false;
		}
	}
	return func(token);
}

static void print_level(const KarToken* token, FILE* stream, size_t level) {
	size_t n;
	
	n = level;
	while (n) {
		fprintf(stream, "\t");
		n--;
	}
	
	if (token->str == NULL) {
		fprintf(stream, "%s(%d, %d): --\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column);
	} else {
		fprintf(stream, "%s(%d, %d): [%s]\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	}
	
	for (n = 0; n < kar_token_child_count(token); ++n) {
		print_level(kar_token_child_get(token, n), stream, level + 1);
	}
}

size_t kar_token_child_find(KarToken* token, const KarTokenType type) {
	size_t pos;
	for (pos = 0; pos < kar_token_child_count(token); ++pos) {
		if (kar_token_child_get(token, pos)->type == type) {
			break;
		}
	}
	return pos;
}

static void restore_str(KarToken* token) {
	// TODO: Возможно стоит дописать для других типов.
	if (token->str != NULL) {
		return;
	}
	if (token->type == KAR_TOKEN_SIGN_GET_FIELD) {
		kar_token_set_str(token, ".");
	} else if (token->type == KAR_TOKEN_SIGN_PLUS) {
		kar_token_set_str(token, "+");
	} else if (token->type == KAR_TOKEN_SIGN_MINUS) {
		kar_token_set_str(token, "-");
	} else if (token->type == KAR_TOKEN_SIGN_SINGLE_PLUS) {
		kar_token_set_str(token, "+");
	} else if (token->type == KAR_TOKEN_SIGN_SINGLE_MINUS) {
		kar_token_set_str(token, "-");
	}
}

KarToken* kar_token_join_children(KarToken* token, size_t first, size_t count) {
	KarToken* first_token = kar_token_child_get(token, first);
	restore_str(first_token);

	for (size_t i = 0; i < count - 1; ++i) {
		KarToken* next_token = kar_token_child_get(token, first + 1);
		restore_str(next_token);

		kar_token_add_str(first_token, next_token->str);

		while (!kar_token_child_empty(next_token)) {
			KarToken* child = kar_token_child_tear(next_token, 0);
			kar_token_child_add(first_token, child);
		}

		kar_token_child_erase(token, first + 1);
	}
	return first_token;
}

KarToken* kar_token_get_first_grandchild(KarToken* token) {
	KarToken* result = token;
	while (!kar_token_child_empty(result)) {
		result = kar_token_child_get(result, 0);
	}
	return result;
}

void kar_token_print(const KarToken* token, FILE* stream) {
	print_level(token, stream, 0);
}

static int get_print_level_size(const KarToken* token, size_t level) {
	int result = 0;
	result += (int)(level * sizeof(KarString));
	
	if (token->str == NULL) {
		result += snprintf(NULL, 0, "%s(%d, %d): --\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column);
	} else {
		result += snprintf(NULL, 0, "%s(%d, %d): [%s]\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	}
	
	size_t n;
	for (n = 0; n < kar_token_child_count(token); ++n) {
		result += get_print_level_size(kar_token_child_get(token, n), level + 1);
	}
	return result;
}

static KarString* create_print_level(const KarToken* token, KarString* buffer, size_t level) {
	size_t n;
	
	n = level;
	while (n) {
		buffer += sprintf(buffer, "\t");
		n--;
	}
	
	if (token->str == NULL) {
		buffer += sprintf(buffer, "%s(%d, %d): --\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column);
	} else {
		buffer += sprintf(buffer, "%s(%d, %d): [%s]\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	}
	
	for (n = 0; n < kar_token_child_count(token); ++n) {
		buffer = create_print_level(kar_token_child_get(token, n), buffer, level + 1);
	}
	return buffer;
}

KarString* kar_token_create_print(const KarToken* token) {
	size_t size = (size_t)get_print_level_size(token, 0);
	KAR_CREATES(result, KarString, size + 1);
	create_print_level(token, result, 0);
	result[size] = 0;
	return result;
}
