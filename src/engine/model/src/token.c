/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
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

KAR_TREE_CODE(token_child, KarToken, KarToken, children, &kar_token_free)

KarToken* kar_token_create()
{
	KAR_CREATE(token, KarToken);
	
	token->type = KAR_TOKEN_UNKNOWN;
	kar_cursor_init(&token->cursor);
	token->str = NULL;
	kar_array_init(&token->children);
	
	return token;
}

KarToken* kar_token_create_fill(KarTokenType type, KarCursor cursor, const char* str)
{
	KAR_CREATE(token, KarToken);
	
	token->type = type;
	token->cursor = cursor;
	if (str) {
		size_t length = strlen(str);
		KAR_ALLOCS(token->str, char, length + 1);
		strcpy(token->str, str);
	} else {
		token->str = NULL;
	}
	kar_array_init(&token->children);
	
	return token;
}

void kar_token_free(KarToken* token)
{
	if (token->str) {
		KAR_FREE(token->str);
	}
	kar_array_clear(&token->children, (KarArrayFreeFn*)&kar_token_free);
	KAR_FREE(token);
}

bool kar_token_check_type(const KarToken* token, const KarTokenType type) {
	return token->type == type;
}

bool kar_token_check_type_name(const KarToken* token, const KarTokenType type, const char* string) {
	return token->type == type && !strcmp(token->str, string);
}

void kar_token_set_str(KarToken* token, const char* str) {
	if (token->str) {
		KAR_FREE(token->str);
	}
	if (!str) {
		token->str = NULL;
		return;
	}
	size_t length = strlen(str);
	KAR_ALLOCS(token->str, char, length + 1);
	strcpy(token->str, str);
}

void kar_token_add_str(KarToken* token, const char* str) {
	if (!token->str) {
		kar_token_set_str(token, str);
		return;
	}
	char* new_string = kar_string_create_concat(token->str, str);
	KAR_FREE(token->str);
	token->str = new_string;
}

bool kar_token_child_foreach_bool(KarToken* token, bool(*func)(KarToken* array)) {
	for (size_t i = 0; i < token->children.count; i++) {
		if (!kar_token_child_foreach_bool(token->children.items[i], func)) {
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
	
	for (n = 0; n < token->children.count; ++n) {
		print_level(kar_token_child(token, n), stream, level + 1);
	}
}

size_t kar_token_child_find(KarToken* token, const KarTokenType type) {
	size_t pos;
	for (pos = 0; pos < token->children.count; ++pos) {
		if (kar_token_child(token, pos)->type == type) {
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
	KarToken* first_token = kar_token_child(token, first);
	restore_str(first_token);

	for (size_t i = 0; i < count - 1; ++i) {
		KarToken* next_token = kar_token_child(token, first + 1);
		restore_str(next_token);

		char* join_str = kar_string_create_concat(first_token->str, next_token->str);
		KAR_FREE(first_token->str);
		first_token->str = join_str;

		while (next_token->children.count != 0) {
			KarToken* child = kar_token_child_tear(next_token, 0);
			kar_token_child_add(first_token, child);
		}

		kar_token_child_tear(token, first + 1);
		kar_token_free(next_token);
	}
	return first_token;
}

KarToken* kar_token_get_first_grandchild(KarToken* token) {
	KarToken* result = token;
	while (result->children.count != 0) {
		result = kar_token_child(result, 0);
	}
	return result;
}

void kar_token_print(const KarToken* token, FILE* stream) {
	print_level(token, stream, 0);
}

static int get_print_level_size(const KarToken* token, size_t level) {
	int result = 0;
	result += (int)(level * sizeof(char));
	
	if (token->str == NULL) {
		result += snprintf(NULL, 0, "%s(%d, %d): --\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column);
	} else {
		result += snprintf(NULL, 0, "%s(%d, %d): [%s]\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	}
	
	size_t n;
	for (n = 0; n < token->children.count; ++n) {
		result += get_print_level_size(kar_token_child(token, n), level + 1);
	}
	return result;
}

static char* create_print_level(const KarToken* token, char* buffer, size_t level) {
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
	
	for (n = 0; n < token->children.count; ++n) {
		buffer = create_print_level(kar_token_child(token, n), buffer, level + 1);
	}
	return buffer;
}

char* kar_token_create_print(const KarToken* token) {
	size_t size = (size_t)get_print_level_size(token, 0);
	KAR_CREATES(result, char, size + 1);
	create_print_level(token, result, 0);
	result[size] = 0;
	return result;
}
