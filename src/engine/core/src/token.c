/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "core/alloc.h"
#include "core/string.h"

KToken* k_token_create()
{
	K_CREATE(token, KToken);
	
	token->type = TOKEN_UNKNOWN;
	k_cursor_init(&token->cursor);
	token->str = NULL;
	
	token->children_count = 0;
	token->children_capacity = 0;
	token->children = NULL;
	
	return token;
}

void k_token_free(KToken* token)
{
	if (token->str) {
		K_FREE(token->str);
	}
	size_t n = token->children_count;
	while(n) {
		k_token_free(token->children[n]);
	}
	if (token->children) {
		K_FREE(token->children);
	}
	K_FREE(token);
}

bool k_token_check_type(const KToken* token, const KTokenType type) {
	return token->type == type;
}

bool k_token_check_type_name(const KToken* token, const KTokenType type, const char* string) {
	return token->type == type && !strcmp(token->str, string);
}

void k_token_set_str(KToken* token, const char* str) {
	if (token->str) {
		K_FREE(token->str);
	}
	if (!str) {
		token->str = NULL;
		return;
	}
	size_t length = strlen(str);
	K_ALLOCS(token->str, char, length + 1);
	strcpy(token->str, str);
}

void k_token_add_str(KToken* token, const char* str) {
	if (!token->str) {
		k_token_set_str(token, str);
		return;
	}
	char* new_string = k_string_concat(token->str, str);
	K_FREE(token->str);
	token->str = new_string;
}

static void k_token_wide_capacity(KToken* token) {
	size_t new_children_capacity;
	if (!token->children_capacity) {
		new_children_capacity = 1;
	} else {
		new_children_capacity = token->children_capacity * 2;
	}
	K_CREATES(new_children, KToken*, new_children_capacity);
	if (token->children) {
		size_t n = token->children_count;
		while (n) {
			n--;
			new_children[n] = token->children[n];
		}
		K_FREE(token->children);
	}
	token->children_capacity = new_children_capacity;
	token->children = new_children;
}

void k_token_add_child(KToken* token, KToken* child) {
	if (token->children_count == token->children_capacity) {
		k_token_wide_capacity(token);
	}
	token->children[token->children_count] = child;
	token->children_count++;
}

void k_token_insert_child(KToken* token, KToken* child, size_t num) {
	if (num >= token->children_count) {
		k_token_add_child(token, child);
		return;
	}
	if (token->children_count == token->children_capacity) {
		k_token_wide_capacity(token);
	}
	size_t i;
	for (i = token->children_count; i > num; --i) {
		token->children[i] = token->children[i - 1];
	}
	token->children[num] = child;
	token->children_count++;
}

KToken* k_token_tear_child(KToken* token, size_t num) {
	if (num >= token->children_count) {
		return NULL;
	}
	KToken* teared = token->children[num];
	token->children_count--;
	size_t n;
	for (n = num; n < token->children_count; ++n) {
		token->children[n] = token->children[n + 1];
	}
	return teared;
}

void k_token_erase_child(KToken* token, size_t num) {
	KToken* teared = k_token_tear_child(token, num);
	if (teared) {
		k_token_free(teared);
	}
}

bool k_token_foreach(KToken* token, bool(*fn)(KToken* token)) {
	bool result;
	size_t i;
	for (i = 0; i < token->children_count; i++) {
		result = k_token_foreach(token->children[i], fn);
		if (!result) {
			return false;
		}
	}
	return fn(token);
}

void k_token_print(const KToken* token, FILE* stream) {
	k_token_print_level(token, stream, 0);
}

void k_token_print_level(const KToken* token, FILE* stream, size_t level) {
	size_t n;
	
	n = level;
	while (n) {
		fprintf(stream, "\t");
		n--;
	}
	
	fprintf(stream, "%s(%d, %d): [%s]\n", k_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	
	for (n = 0; n < token->children_count; ++n) {
		k_token_print_level(token->children[n], stream, level + 1);
	}
}

int k_token_snprint_level(const KToken* token, size_t level) {
	int result = 0;
	result += (int)(level * sizeof(char));
	
	result += snprintf(NULL, 0, "%s(%d, %d): [%s]\n", k_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	
	size_t n;
	for (n = 0; n < token->children_count; ++n) {
		result += k_token_snprint_level(token->children[n], level + 1);
	}
	return result;
}

char* k_token_sprint(const KToken* token) {
	size_t size = (size_t)k_token_snprint_level(token, 0);
	K_CREATES(result, char, size);
	k_token_sprint_level(token, result, 0);
	return result;
}

char* k_token_sprint_level(const KToken* token, char* buffer, size_t level) {
	size_t n;
	
	n = level;
	while (n) {
		buffer += sprintf(buffer, "\t");
		n--;
	}
	
	buffer += sprintf(buffer, "%s(%d, %d): [%s]\n", k_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	
	for (n = 0; n < token->children_count; ++n) {
		buffer = k_token_sprint_level(token->children[n], buffer, level + 1);
	}
	return buffer;
}
