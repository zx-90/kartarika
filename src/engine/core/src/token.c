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

KAR_ARRAY_CODE(token_child, KarToken, children, kar_token_free)

KarToken* kar_token_create()
{
	KAR_CREATE(token, KarToken);
	
	token->type = KAR_TOKEN_UNKNOWN;
	kar_cursor_init(&token->cursor);
	token->str = NULL;
	kar_array_init(&token->children);
	
	return token;
}

void kar_token_free(KarToken* token)
{
	if (token->str) {
		KAR_FREE(token->str);
	}
	kar_array_free(&token->children, (KarArrayFreeFn*)&kar_token_free);
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
	
	for (n = 0; n < token->children.count; ++n) {
		kar_token_print_level(kar_token_child(token, n), stream, level + 1);
	}
}

int kar_token_snprint_level(const KarToken* token, size_t level) {
	int result = 0;
	result += (int)(level * sizeof(char));
	
	result += snprintf(NULL, 0, "%s(%d, %d): [%s]\n", kar_token_type_get_name(token->type), token->cursor.line, token->cursor.column, token->str);
	
	size_t n;
	for (n = 0; n < token->children.count; ++n) {
		result += kar_token_snprint_level(kar_token_child(token, n), level + 1);
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
	
	for (n = 0; n < token->children.count; ++n) {
		buffer = kar_token_sprint_level(kar_token_child(token, n), buffer, level + 1);
	}
	return buffer;
}
