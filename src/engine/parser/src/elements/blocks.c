/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool is_empty_line(KarToken* token) {
	for (size_t i = 0; i < token->children.count; ++i) {
		KarToken* child = kar_token_child(token, i);
		if (child->type != KAR_TOKEN_SPACE && child->type != KAR_TOKEN_COMMENT) {
			return false;
		}
	}
	return true;
}

static int get_token_indent(KarToken* token) {
	if (token->type != KAR_TOKEN_INDENT) {
		return -1;
	}
	int indent = 0;
	char* c = token->str;
	while(*c) {
		if (*c == '\t') {
			indent += 4;
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
	int parentIndent)
{
	int indent = get_token_indent(kar_token_child(rootToken, *num));
	int currentIndent = indent;
	size_t current = *num;
	while (true) {
		KarToken* line = kar_token_child_tear(rootToken, current);
		kar_token_child_add(parentToken, line);

		if (current == rootToken->children.count) {
			*num = current;
			return true;
		}
		currentIndent = get_token_indent(kar_token_child(rootToken, current));

		if (currentIndent > indent) {
			KarToken* parent = kar_token_child(parentToken, parentToken->children.count - 1);
			if (!fill_block(rootToken, &current, parent, indent)) {
				return false;
			}
			if (current == rootToken->children.count) {
				*num = current;
				return true;
			}
		} else if (currentIndent < indent) {
			if (currentIndent > parentIndent) {
				return false;
			}
			*num = current;
			return true;
		}
	}
	return true;
}

bool kar_parser_split_by_blocks(KarToken* token) {
	for (size_t i = 0; i < token->children.count; ++i) {
		if (!is_empty_line(kar_token_child(token, i))) {
			if (get_token_indent(kar_token_child(token, i)) != 0) {
				size_t curChild = i;
				if (!fill_block(token, &curChild, (KarToken*)token->children.items[i-1], 0)) {
					return false;
				}
			}
		}
	}
	return true;
}
