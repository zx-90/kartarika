/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/first_lexer.h"

#include <stdlib.h>
#include <string.h>

#include "core/module_error.h"

const char* ERROR_READ_STREAM = "Не могу прочитать модуль. Нет доступа к модулю, он повреждён или удалён.";
const char* ERROR_SYMBOL_STRING = "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.";
const char* ERROR_UNKNOWN_SYMBOL = "Обнаружен неожиданный символ при разборе файла";
const char* ERROR_END_OF_FILE_PARSING_STRING = "Неожиданный конец файла при разборе строки. Возможно забыты закрывающие строку кавычки.";
const char* ERROR_END_OF_FILE_PARSING_COMMENT = "Неожиданный конец файла при разборе многострочного комментария. Возможно забыт символ \"!\" для завершения комментария в конце файла.";

KFirstLexer* k_first_lexer_create(KStream* stream, KModule* module) {
	KFirstLexer* lexer = (KFirstLexer*)malloc(sizeof(KFirstLexer));
	if (!lexer) {
		return NULL;
	}
	
	lexer->streamCursor = k_stream_cursor_create(stream);
	if (!lexer->streamCursor) {
		free(lexer);
		return NULL;
	}
	
	lexer->current = k_token_create();
	if (!lexer->current) {
		free(lexer);
		return NULL;
	}
	lexer->current->cursor = lexer->streamCursor->cursor;
	lexer->current->type = TOKEN_INDENT;
	k_token_set_str(lexer->current, "");
	
	lexer->module = module;
	lexer->status = STATUS_INDENT;
	
	return lexer;
}

void k_first_lexer_free(KFirstLexer* lexer) {
	free(lexer->current);
	free(lexer->streamCursor);
	free(lexer);
}

// -----------------------------------
static char* string_to_hex(const char* input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = strlen(input);

    char* output = (char*)malloc(sizeof(char) * len * 2 + 1);
	char* cur = output;
	while(!*input) {
		*cur++ = lut[*input >> 4];
		*cur++ = lut[*input & 15];
		input++;
	}
	*cur = 0;
    return output;
}

static const char* SPACES[] = {" ", "\t", "\r"};
static const char* SIGNS[] = {"(", ")", ".", "+", "-", "*", "/", "%", ",", "=", ":", "?"};

static bool is_space(KFirstLexer* lexer) {
	return k_stream_cursor_is_one_of(lexer->streamCursor, SPACES, sizeof(SPACES) / sizeof(char*));
}

static bool is_identifier(KFirstLexer* lexer) {
	const char* str = k_stream_cursor_get(lexer->streamCursor);
	return
		(strcmp(str, "0") >= 0 && strcmp(str, "9") <= 0) ||
		(strcmp(str, "a") >= 0 && strcmp(str, "z") <= 0) ||
		(strcmp(str, "A") >= 0 && strcmp(str, "Z") <= 0) ||
		(strcmp(str, "_") == 0) ||
		// Кириллица
		(strcmp(str, "\u0400") >= 0 && strcmp(str, "\u052F") <= 0);
}

static bool is_sign(KFirstLexer* lexer) {
	return k_stream_cursor_is_one_of(lexer->streamCursor, SIGNS, sizeof(SIGNS) / sizeof(char*));
}

static KLexerStatus get_status_by_symbol(KFirstLexer* lexer) {
	if (is_space(lexer)) {
		return STATUS_SPACE;
	}
	if (is_identifier(lexer)) {
		return STATUS_IDENTIFIER;
	}
	if (is_sign(lexer)) {
		return STATUS_SIGN;
	}
	char buff[1024];
	snprintf(buff, sizeof(buff), "%s: %s (код %s).", ERROR_UNKNOWN_SYMBOL, k_stream_cursor_get(lexer->streamCursor), string_to_hex(k_stream_cursor_get(lexer->streamCursor)));
	k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, buff);
	return STATUS_UNKNOWN;
}

// ------------------------------
static void new_token(KFirstLexer* lexer, KTokenType type) {
	lexer->current = k_token_create();
	lexer->current->type = type;
	lexer->current->cursor = lexer->streamCursor->cursor;
	k_token_set_str(lexer->current, "");
}

static void push_token(KFirstLexer* lexer) {
	if (strcmp(lexer->current->str, "") || lexer->current->type != TOKEN_UNKNOWN) {
		k_token_add_child(lexer->module->token, lexer->current);
	}
	new_token(lexer, TOKEN_UNKNOWN);
}

static void next_token(KFirstLexer* lexer, KTokenType type, KLexerStatus st) {
	push_token(lexer);
	new_token(lexer, type);
	lexer->status = st;
}

static void next_token_default(KFirstLexer* lexer, KLexerStatus st) {
	if (st == STATUS_SPACE) {
		next_token(lexer, TOKEN_SPACE, st);
	} else if (st == STATUS_IDENTIFIER) {
		next_token(lexer, TOKEN_IDENTIFIER, st);
	} else if (st == STATUS_SIGN) {
		next_token(lexer, TOKEN_SIGN, st);
	} else {
		next_token(lexer, TOKEN_UNKNOWN, st);
	}
}

static void add_new_line(KFirstLexer* lexer) {
	push_token(lexer);
	new_token(lexer, TOKEN_INDENT);
	lexer->current->cursor = lexer->streamCursor->cursor;
	lexer->status = STATUS_INDENT;
}


static void parse_string(KFirstLexer* lexer) {
	// TODO: Сделать парсинг спецсимволов \н, \т, \\ и т.п.
	while (true) {
		if (k_stream_cursor_is_eof(lexer->streamCursor)) {
			k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_END_OF_FILE_PARSING_STRING);
			return;
		}
		if (!k_stream_cursor_next(lexer->streamCursor)) {
			k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (k_stream_cursor_is_equal(lexer->streamCursor, "\"")) {
			next_token_default(lexer, STATUS_UNKNOWN);
			return;
		}
		if (k_stream_cursor_is_equal(lexer->streamCursor, "%")) {
			k_token_add_str(lexer->current, k_stream_cursor_get(lexer->streamCursor));
			if (!k_stream_cursor_next(lexer->streamCursor)) {
				k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
			}
			k_token_add_str(lexer->current, k_stream_cursor_get(lexer->streamCursor));
		}
		k_token_add_str(lexer->current, k_stream_cursor_get(lexer->streamCursor));
	}
}

static void parse_singleline_comment(KFirstLexer* lexer) {
	while (true) {
		if (k_stream_cursor_is_eof(lexer->streamCursor)) {
			return;
		}
		if (!k_stream_cursor_next(lexer->streamCursor)) {
			k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (k_stream_cursor_is_equal(lexer->streamCursor, "\n")) {
			next_token_default(lexer, STATUS_UNKNOWN);
			return;
		}
		k_token_add_str(lexer->current, k_stream_cursor_get(lexer->streamCursor));
	}
}

static void parse_multiline_comment(KFirstLexer* lexer) {
	while (true) {
		if (k_stream_cursor_is_eof(lexer->streamCursor)) {
			k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_END_OF_FILE_PARSING_COMMENT);
			return;
			
		}
		if (!k_stream_cursor_next(lexer->streamCursor)) {
			k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (k_stream_cursor_is_equal(lexer->streamCursor, "!")) {
			next_token_default(lexer, STATUS_UNKNOWN);
			return;
		}
		k_token_add_str(lexer->current, k_stream_cursor_get(lexer->streamCursor));
	}
}

bool k_first_lexer_run(KFirstLexer* lexer) {
	KStreamCursor* streamCursor = lexer->streamCursor;
	while (!k_stream_cursor_is_eof(streamCursor)) {
		if (!k_stream_cursor_is_good(streamCursor)) {
			k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_READ_STREAM);
			return false;
		}
		if (!k_stream_cursor_next(streamCursor)) {
			next_token(lexer, TOKEN_UNKNOWN, STATUS_UNKNOWN);
			k_token_set_str(lexer->current, k_stream_cursor_get(streamCursor));
			k_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (k_stream_cursor_is_eof(streamCursor)) {
			break;
		}
		if (k_stream_cursor_is_equal(streamCursor, "\"")) {
			next_token(lexer, TOKEN_VAL_STRING, STATUS_UNKNOWN);
			parse_string(lexer);
			continue;
		}
		if (k_stream_cursor_is_equal(streamCursor, "№")) {
			next_token(lexer, TOKEN_COMMENT, STATUS_UNKNOWN);
			parse_singleline_comment(lexer);
			continue;
		}
		if (k_stream_cursor_is_equal(streamCursor, "!")) {
			next_token(lexer, TOKEN_COMMENT, STATUS_UNKNOWN);
			parse_multiline_comment(lexer);
			continue;
		}
		if (k_stream_cursor_is_equal(streamCursor, "\n")) {
			add_new_line(lexer);
			continue;
		}
		if (lexer->status == STATUS_INDENT) {
			KLexerStatus st = get_status_by_symbol(lexer);
			if (st != STATUS_SPACE) {
				next_token_default(lexer, st);
			}
		} else if (lexer->status == STATUS_SPACE) {
			KLexerStatus st = get_status_by_symbol(lexer);
			if (st != STATUS_SPACE) {
				next_token_default(lexer, st);
			}
		} else if (lexer->status == STATUS_IDENTIFIER) {
			KLexerStatus st = get_status_by_symbol(lexer);
			if (st != STATUS_IDENTIFIER) {
				next_token_default(lexer, st);
			}
		} else if (lexer->status == STATUS_SIGN) {
			KLexerStatus st = get_status_by_symbol(lexer);
			next_token_default(lexer, st);
		} else if (lexer->status == STATUS_UNKNOWN) {
			if (is_space(lexer)) {
				next_token_default(lexer, STATUS_SPACE);
			}
			if (is_sign(lexer)) {
				next_token_default(lexer, STATUS_SIGN);
			}
		}

		k_token_add_str(lexer->current, k_stream_cursor_get(streamCursor));
	}
	push_token(lexer);
	return k_module_error_get_count() == 0;
	
}
