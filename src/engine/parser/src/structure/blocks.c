/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include <stdbool.h>

#include "model/token.h"
#include "model/project_error_list.h"

//-----------------------------------------------------------------------------
typedef struct {
	KarToken* token;
	size_t num;
} KarTokenCursor;

typedef struct {
	KarToken* token;
	int indent;
} KarTokenIndent;

//-----------------------------------------------------------------------------

static bool is_empty_token(KarToken* token) {
	return token->type == KAR_TOKEN_SPACE || token->type == KAR_TOKEN_COMMENT;
}

static bool is_colon_end(KarToken* token) {
	for (size_t i = kar_token_child_count(token); i > 0; --i) {
		KarToken* child = kar_token_child_get(token, i - 1);
		if (is_empty_token(child)) {
			continue;
		}
		return child->type == KAR_TOKEN_SIGN_COLON;
	}
	return false;
}

static KarToken* get_first_not_empty(KarToken* token) {
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (is_empty_token(child)) {
			continue;
		}
		return child;
	}
	return NULL;
}

static KarToken* get_last_not_empty(KarToken* token) {
	for (size_t i = kar_token_child_count(token); i > 0; --i) {
		KarToken* child = kar_token_child_get(token, i - 1);
		if (is_empty_token(child)) {
			continue;
		}
		return child;
	}
	return NULL;
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

static KarToken* cursor_current(KarTokenCursor cursor) {
	return kar_token_child_get(cursor.token, cursor.num);
}

static KarTokenIndent get_current_indent(KarTokenCursor root) {
	KarToken* line = cursor_current(root);
	KarTokenIndent current = {line, get_token_indent(line)};
	return current;
}

static KarToken* get_parent(
	KarTokenCursor root,
	KarTokenIndent parentToken,
	KarProjectErrorList* errors
	)
{
	KarToken* parent;
	if (root.token == parentToken.token) {
		parent = kar_token_child_get(root.token, root.num - 1);
	} else {
		parent = kar_token_child_get_last(parentToken.token, 0);
	}
	
	KarToken* last_not_empty = get_last_not_empty(parent);
	if (last_not_empty != NULL && last_not_empty->type != KAR_TOKEN_SIGN_COLON) {
		kar_project_error_list_create_add(errors, &last_not_empty->cursor, 1, "Строка заголовка блока должна заканчиваться двоеточием.");
		return NULL;
	}
	return parent;
}

static bool fill_block(KarTokenCursor root, KarTokenIndent parentToken, KarProjectErrorList* errors);

static bool fill_subblock(KarTokenCursor root, KarTokenIndent parentToken, int indent, KarProjectErrorList* errors) {
	KarToken* line = cursor_current(root);
	
	KarToken* parent = get_parent(root, parentToken, errors);
	if (parent == NULL) {
		return false;
	}
	
	// TODO: Курсор неправильно определяется для выражений. Надо брать курсор из первого наследника первого наследника и т.д.
	//       Тоже самое посмотреть для скобок, вызова метода и других элементов, внутри которых может находиться выражение.
	KarToken* block = kar_token_create_fill(KAR_TOKEN_BLOCK_BODY, line->cursor, NULL);
	kar_token_child_add(parent, block);
	KarTokenIndent block_indent = {block, indent};
	if (!fill_block(root, block_indent, errors)) {
		return false;
	}
	if (kar_token_child_empty(block)) {
		kar_project_error_list_create_add(errors, &get_first_not_empty(line)->cursor, 1, "Не найдена ни одна команда в блоке.");
		return false;
	}
	return true;
}

static void fill_block_line(KarTokenCursor* root, KarTokenIndent parent) {
	if (root->token == parent.token) {
		root->num++;
	} else {
		kar_token_child_move_to_end(root->token, parent.token, root->num, 1);
	}
}

static bool fill_block(KarTokenCursor root, KarTokenIndent parent, KarProjectErrorList* errors)
{
	bool prev_colon_end = false;
	int indent = get_token_indent(cursor_current(root));
	
	while (root.num != kar_token_child_count(root.token)) {
		KarTokenIndent current = get_current_indent(root);

		if (current.indent > indent) {
			if (!fill_subblock(root, parent, indent, errors)) {
				return false;
			}
			prev_colon_end = false;
			continue;
		}
		
		if (prev_colon_end) {
			kar_project_error_list_create_add(errors, &get_first_not_empty(current.token)->cursor, 1, "Не найдена ни одна команда в блоке.");
			return false;
		}
		prev_colon_end = is_colon_end(current.token);
		
		if (current.indent == indent) {
			fill_block_line(&root, parent);
			continue;
		}
		
		if (current.indent < indent) {
			if (current.indent > parent.indent) {
				kar_project_error_list_create_add(errors, &get_first_not_empty(current.token)->cursor, 1, "Отступ в строке не соответствует ни одному предыдущему отступу.");
				return false;
			}
			return true;
		}
	}
	return !prev_colon_end;
}

bool kar_parser_split_by_blocks(KarToken* token, KarProjectErrorList* errors) {
	if (kar_token_child_empty(token)) {
		return true;
	}
	
	KarTokenCursor cursor = {token, 0};
	KarTokenIndent indent = {token, get_token_indent(cursor_current(cursor))};
	if (indent.indent != 0) {
		kar_project_error_list_create_add(errors, &get_first_not_empty(cursor_current(cursor))->cursor, 1, "Отступ в строке не соответствует ни одному предыдущему отступу.");
		return false;
	}
	
	return fill_block(cursor, indent, errors);
}
