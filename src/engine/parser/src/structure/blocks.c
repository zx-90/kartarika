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
	bool prev_colon_end = false;
	int indent = get_token_indent(kar_token_child(rootToken, *num));
	int currentIndent = indent;
	while (true) {
		if (*num == rootToken->children.count) {
			return true;
		}
		KarToken* line = kar_token_child(rootToken, *num);
		if (is_empty_line(line)) {
			kar_token_child_erase(rootToken, *num);
			continue;
		}

		currentIndent = get_token_indent(kar_token_child(rootToken, *num));

		if (currentIndent > indent) {
			if (*num == 0) {
				kar_module_error_create_add(errors, &line->cursor, 1, "Отступ в строке не соответствует ни одному предыдущему отступу.");
				return false;
			}
			KarToken* parent;
			if (rootToken == parentToken) {
				parent = kar_token_child(rootToken, (*num) - 1);
			} else {
				parent = kar_token_child(parentToken, parentToken->children.count - 1);
			}
			if (!is_colon_end(parent)) {
				kar_module_error_create_add(errors, &parent->cursor, 1, "Блок должен открываться двоеточием.");
				return false;
			}
			KarToken* block = kar_token_create();
			block->type = KAR_TOKEN_BLOCK_BODY;
			block->cursor = line->cursor;
			kar_token_child_add(parent, block);
			if (!fill_block(rootToken, num, block, indent, errors)) {
				return false;
			}
			if (block->children.count == 0) {
				kar_module_error_create_add(errors, &line->cursor, 1, "Не найдена ни одна команда в блоке.");
				return false;
			}
			prev_colon_end = false;
			continue;
		}
		
		if (prev_colon_end) {
			kar_module_error_create_add(errors, &line->cursor, 1, "Не найдена ни одна команда в блоке.");
			return false;
		}
		prev_colon_end = is_colon_end(line);
		
		if (currentIndent == indent) {
			if (rootToken == parentToken) {
				*num = *num + 1;
			} else {
				kar_token_child_tear(rootToken, *num);
				kar_token_child_add(parentToken, line);
			}
			continue;
		}
		
		if (currentIndent < indent) {
			if (currentIndent > parentIndent) {
				kar_module_error_create_add(errors, &kar_token_child(rootToken, *num)->cursor, 1, "Отступ в строке не соответствует ни одному предыдущему отступу.");
				return false;
			}
			return true;
		}
	}
	return true;
}

bool kar_parser_split_by_blocks(KarToken* token, KarArray* errors) {
	if (token->children.count == 0) {
		kar_module_error_create_add(errors, 0, 1, "Внутрення ошибка. В модуле нет ни одного токена.");
		return false;
	}
	if (token->children.count == 1 && kar_token_child(token, 0)->type == KAR_TOKEN_INDENT) {
		return true;
	}
	size_t curChild = 0;
	return fill_block(token, &curChild, token, 0, errors);
}
