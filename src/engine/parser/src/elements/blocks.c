/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"
#include "core/module_error.h"

static bool is_empty_token(KarToken* token) {
	return token->type == KAR_TOKEN_SPACE || token->type == KAR_TOKEN_COMMENT;
}

static bool is_empty_line(KarToken* token) {
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		if (!is_empty_token(child)) {
			return false;
		}
	}
	return true;
}

static bool is_colon_end(KarToken* token) {
	for (size_t i = token->children.count; i > 0; --i) {
		KarToken* child = kar_token_child(token, i - 1);
		if (is_empty_token(child)) {
			continue;
		}
		return child->type == KAR_TOKEN_SIGN_COLON;
	}
	return false;
}

static int get_token_indent(KarToken* token) {
	if (token->type != KAR_TOKEN_INDENT) {
		return -1;
	}
	int indent = 0;
	char* c = token->str;
	while(*c) {
		if (*c == '\t') {
			indent = (indent + 4) / 4 * 4;
		} else {
			indent++;
		}
		c++;
	}
	return indent;
}

static bool fill_block(
	KarToken* rootToken,
	size_t* num,
	KarToken* parentToken,
	int parentIndent,
	KarArray* errors
	)
{
	KarToken* first_child = kar_token_child(rootToken, *num);
	if (!is_colon_end(parentToken)) {
		kar_module_error_create_add(errors, &first_child->cursor, 1, "Блок должен открываться двоеточием в конце предыдущей строки.");
		return false;
	}
	int indent = get_token_indent(first_child);
	int currentIndent = indent;
	size_t current = *num;
	KarToken* block = kar_token_create();
	block->type = KAR_TOKEN_BLOCK_BODY;
	block->cursor = first_child->cursor;
	kar_token_child_add(parentToken, block);
	while (true) {
		KarToken* line = kar_token_child_tear(rootToken, current);
		if (is_empty_line(line)) {
			kar_token_free(line);
			continue;
		}
		kar_token_child_add(block, line);

		if (current == rootToken->children.count) {
			*num = current;
			return true;
		}
		currentIndent = get_token_indent(kar_token_child(rootToken, current));

		if (currentIndent > indent) {
			KarToken* parent = kar_token_child(block, block->children.count - 1);
			if (!fill_block(rootToken, &current, parent, indent, errors)) {
				return false;
			}
			if (current == rootToken->children.count) {
				*num = current;
				return true;
			}
		} else if (currentIndent < indent) {
			if (currentIndent > parentIndent) {
				kar_module_error_create_add(errors, &kar_token_child(rootToken, current)->cursor, 1, "Отступ в строке не соответствует ни одному предыдущему отступу.");
				return false;
			}
			*num = current;
			return true;
		}
	}
	return true;
}

bool kar_parser_split_by_blocks(KarToken* token, KarArray* errors) {
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		if (is_empty_line(child)) {
			kar_token_child_erase(token, i);
			i--;
			continue;
		}
		if (get_token_indent(child) != 0) {
			if (i == 0) {
				kar_module_error_create_add(errors, &child->cursor, 1, "Отступ в строке не соответствует ни одному предыдущему отступу.");
				return false;
			}
			size_t curChild = i;
			if (!fill_block(token, &curChild, (KarToken*)token->children.items[i-1], 0, errors)) {
				return false;
			}
		}
	}
	return true;
}
