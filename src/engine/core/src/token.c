/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"

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
	
	token->children_count = 0;
	token->children_capacity = 0;
	token->children = NULL;
	
	return token;
}

void kar_token_free(KarToken* token)
{
	if (token->str) {
		KAR_FREE(token->str);
	}
	size_t n = token->children_count;
	while(n) {
		kar_token_free(token->children[n]);
	}
	if (token->children) {
		KAR_FREE(token->children);
	}
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
	char* new_string = kar_string_concat(token->str, str);
	KAR_FREE(token->str);
	token->str = new_string;
}

static void kar_token_wide_capacity(KarToken* token) {
	size_t new_children_capacity;
	if (!token->children_capacity) {
		new_children_capacity = 1;
	} else {
		new_children_capacity = token->children_capacity * 2;
	}
	KAR_CREATES(new_children, KarToken*, new_children_capacity);
	if (token->children) {
		size_t n = token->children_count;
		while (n) {
			n--;
			new_children[n] = token->children[n];
		}
		KAR_FREE(token->children);
	}
	token->children_capacity = new_children_capacity;
	token->children = new_children;
}

void kar_token_add_child(KarToken* token, KarToken* child) {
	if (token->children_count == token->children_capacity) {
		kar_token_wide_capacity(token);
	}
	token->children[token->children_count] = child;
	token->children_count++;
}

void kar_token_insert_child(KarToken* token, KarToken* child, size_t num) {
	if (num >= token->children_count) {
		kar_token_add_child(token, child);
		return;
	}
	if (token->children_count == token->children_capacity) {
		kar_token_wide_capacity(token);
	}
	size_t i;
	for (i = token->children_count; i > num; --i) {
		token->children[i] = token->children[i - 1];
	}
	token->children[num] = child;
	token->children_count++;
}

KarToken* kar_token_tear_child(KarToken* token, size_t num) {
	if (num >= token->children_count) {
		return NULL;
	}
	KarToken* teared = token->children[num];
	token->children_count--;
	size_t n;
	for (n = num; n < token->children_count; ++n) {
		token->children[n] = token->children[n + 1];
	}
	return teared;
}

void kar_token_erase_child(KarToken* token, size_t num) {
	KarToken* teared = kar_token_tear_child(token, num);
	if (teared) {
		kar_token_free(teared);
	}
}

bool kar_token_foreach(KarToken* token, bool(*fn)(KarToken* token)) {
	bool result;
	size_t i;
	for (i = 0; i < token->children_count; i++) {
		result = kar_token_foreach(token->children[i], fn);
		if (!result) {
			return false;
		}
	}
	return fn(token);
}

void kar_token_print(const KarToken* token, FILE* stream) {
	kar_token_print_level(token, stream, 0);
}

void kar_token_print_level(const KarToken* token, FILE* stream, size_t level) {
	size_t n;
	
	n = level;
	while (n) {
		fprintf(stream, "\t");
		n--;
	}
	
	fprintf(stream, "%s(%d, %d): [%s]\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	
	for (n = 0; n < token->children_count; ++n) {
		kar_token_print_level(token->children[n], stream, level + 1);
	}
}

int kar_token_snprint_level(const KarToken* token, size_t level) {
	int result = 0;
	result += (int)(level * sizeof(char));
	
	result += snprintf(NULL, 0, "%s(%d, %d): [%s]\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	
	size_t n;
	for (n = 0; n < token->children_count; ++n) {
		result += kar_token_snprint_level(token->children[n], level + 1);
	}
	return result;
}

char* kar_token_sprint(const KarToken* token) {
	size_t size = (size_t)kar_token_snprint_level(token, 0);
	KAR_CREATES(result, char, size);
	kar_token_sprint_level(token, result, 0);
	return result;
}

char* kar_token_sprint_level(const KarToken* token, char* buffer, size_t level) {
	size_t n;
	
	n = level;
	while (n) {
		buffer += sprintf(buffer, "\t");
		n--;
	}
	
	buffer += sprintf(buffer, "%s(%d, %d): [%s]\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	
	for (n = 0; n < token->children_count; ++n) {
		buffer = kar_token_sprint_level(token->children[n], buffer, level + 1);
	}
	return buffer;
}
