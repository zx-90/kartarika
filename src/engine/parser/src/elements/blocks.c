/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "core/token.h"

static bool is_empty_line(KToken* token) {
	for (size_t i = 0; i < token->children_count; ++i) {
		if (token->children[i]->type != TOKEN_SPACE && token->children[i]->type != TOKEN_COMMENT) {
			return false;
		}
	}
	return true;
}

static int get_token_indent(KToken* token) {
	if (token->type != TOKEN_INDENT) {
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
	KToken* rootToken,
	size_t* num,
	KToken* parentToken,
	int parentIndent)
{
	int indent = get_token_indent(rootToken->children[*num]);
	int currentIndent = indent;
	size_t current = *num;
	while (true) {
		KToken* line = k_token_tear_child(rootToken, current);
		k_token_add_child(parentToken, line);

		if (current == rootToken->children_count) {
			*num = current;
			return true;
		}
		currentIndent = get_token_indent(rootToken->children[current]);

		if (currentIndent > indent) {
			KToken* parent = parentToken->children[parentToken->children_count - 1];
			if (!fill_block(rootToken, &current, parent, indent)) {
				return false;
			}
			if (current == rootToken->children_count) {
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

bool k_parser_split_by_blocks(KToken* token) {
	for (size_t i = 0; i < token->children_count; ++i) {
		if (!is_empty_line(token->children[i])) {
			if (get_token_indent(token->children[i]) != 0) {
				size_t curChild = i;
				if (!fill_block(token, &curChild, token->children[i-1], 0)) {
					return false;
				}
			}
		}
	}
	return true;
}
