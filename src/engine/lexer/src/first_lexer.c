/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/first_lexer.h"

#include <string.h>

#include "core/alloc.h"
#include "core/keyword.h"
#include "core/module_error.h"

const char* ERROR_READ_STREAM = "Не могу прочитать модуль. Нет доступа к модулю, он повреждён или удалён.";
const char* ERROR_SYMBOL_STRING = "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.";
const char* ERROR_UNKNOWN_SYMBOL = "Обнаружен неожиданный символ при разборе файла";
const char* ERROR_END_OF_FILE_PARSING_STRING = "Неожиданный конец файла при разборе строки. Возможно забыты закрывающие строку кавычки.";
const char* ERROR_END_OF_FILE_PARSING_COMMENT = "Неожиданный конец файла при разборе многострочного комментария. Возможно забыт символ \"!\" для завершения комментария в конце файла.";

KarFirstLexer* kar_first_lexer_create(KarStream* stream, KarModule* module) {
	KAR_CREATE(lexer, KarFirstLexer);
	if (!lexer) {
		return NULL;
	}
	
	lexer->streamCursor = kar_stream_cursor_create(stream);
	if (!lexer->streamCursor) {
		KAR_FREE(lexer);
		return NULL;
	}
	
	lexer->current = kar_token_create();
	if (!lexer->current) {
		KAR_FREE(lexer);
		return NULL;
	}
	lexer->current->cursor = lexer->streamCursor->cursor;
	lexer->current->type = KAR_TOKEN_INDENT;
	kar_token_set_str(lexer->current, "");
	
	lexer->module = module;
	lexer->status = KAR_LEXER_STATUS_INDENT;
	
	return lexer;
}

void kar_first_lexer_free(KarFirstLexer* lexer) {
	KAR_FREE(lexer->current);
	KAR_FREE(lexer->streamCursor);
	KAR_FREE(lexer);
}

// -----------------------------------
static char* string_to_hex(const char* input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = strlen(input);

	KAR_CREATES(output, char, len * 2 + 1);
	char* cur = output;
	while(!*input) {
		*cur++ = lut[*input >> 4];
		*cur++ = lut[*input & 15];
		input++;
	}
	*cur = 0;
    return output;
}

static const char* SPACES[] = {
	KAR_KEYWORD_SPACE,
	KAR_KEYWORD_SPACE_TAB,
	KAR_KEYWORD_SPACE_CARRIAGE_RETURN
};

static const char* SIGNS[] = {
	KAR_KEYWORD_SIGN_OPEN_BRACES,
	KAR_KEYWORD_SIGN_CLOSE_BRACES,
	KAR_KEYWORD_SIGN_NULLABLE,
	KAR_KEYWORD_SIGN_GET_FIELD,
	KAR_KEYWORD_SIGN_COMMA,
	KAR_KEYWORD_SIGN_COLON,
	KAR_KEYWORD_SIGN_ASSIGN,
	KAR_KEYWORD_SIGN_PLUS,
	KAR_KEYWORD_SIGN_MINUS,
	KAR_KEYWORD_SIGN_MUL,
	KAR_KEYWORD_SIGN_DIV,
	KAR_KEYWORD_SIGN_MOD
};

static bool is_space(KarFirstLexer* lexer) {
	return kar_stream_cursor_is_one_of(lexer->streamCursor, SPACES, sizeof(SPACES) / sizeof(char*));
}

static bool is_identifier(KarFirstLexer* lexer) {
	const char* str = kar_stream_cursor_get(lexer->streamCursor);
	return
		(strcmp(str, "0") >= 0 && strcmp(str, "9") <= 0) ||
		(strcmp(str, "a") >= 0 && strcmp(str, "z") <= 0) ||
		(strcmp(str, "A") >= 0 && strcmp(str, "Z") <= 0) ||
		(strcmp(str, "_") == 0) ||
		// Кириллица
		(strcmp(str, "\u0400") >= 0 && strcmp(str, "\u052F") <= 0);
}

static bool is_sign(KarFirstLexer* lexer) {
	return kar_stream_cursor_is_one_of(lexer->streamCursor, SIGNS, sizeof(SIGNS) / sizeof(char*));
}

static KarLexerStatus get_status_by_symbol(KarFirstLexer* lexer) {
	if (is_space(lexer)) {
		return KAR_LEXER_STATUS_SPACE;
	}
	if (is_identifier(lexer)) {
		return KAR_LEXER_STATUS_IDENTIFIER;
	}
	if (is_sign(lexer)) {
		return KAR_LEXER_STATUS_SIGN;
	}
	char buff[1024];
	snprintf(buff, sizeof(buff), "%s: %s (код %s).", ERROR_UNKNOWN_SYMBOL, kar_stream_cursor_get(lexer->streamCursor), string_to_hex(kar_stream_cursor_get(lexer->streamCursor)));
	kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, buff);
	return KAR_LEXER_STATUS_UNKNOWN;
}

// ------------------------------
static void new_token(KarFirstLexer* lexer, KarTokenType type) {
	lexer->current = kar_token_create();
	lexer->current->type = type;
	lexer->current->cursor = lexer->streamCursor->cursor;
	kar_token_set_str(lexer->current, "");
}

static void push_token(KarFirstLexer* lexer) {
	if (strcmp(lexer->current->str, "") || lexer->current->type != KAR_TOKEN_UNKNOWN) {
		kar_token_add_child(lexer->module->token, lexer->current);
	}
	new_token(lexer, KAR_TOKEN_UNKNOWN);
}

static void next_token(KarFirstLexer* lexer, KarTokenType type, KarLexerStatus st) {
	push_token(lexer);
	new_token(lexer, type);
	lexer->status = st;
}

static void next_token_default(KarFirstLexer* lexer, KarLexerStatus st) {
	if (st == KAR_LEXER_STATUS_SPACE) {
		next_token(lexer, KAR_TOKEN_SPACE, st);
	} else if (st == KAR_LEXER_STATUS_IDENTIFIER) {
		next_token(lexer, KAR_TOKEN_IDENTIFIER, st);
	} else if (st == KAR_LEXER_STATUS_SIGN) {
		next_token(lexer, KAR_TOKEN_SIGN, st);
	} else {
		next_token(lexer, KAR_TOKEN_UNKNOWN, st);
	}
}

static void add_new_line(KarFirstLexer* lexer) {
	push_token(lexer);
	new_token(lexer, KAR_TOKEN_INDENT);
	lexer->current->cursor = lexer->streamCursor->cursor;
	lexer->status = KAR_LEXER_STATUS_INDENT;
}


static void parse_string(KarFirstLexer* lexer) {
	// TODO: Сделать парсинг спецсимволов \н, \т, \\ и т.п.
	while (true) {
		if (kar_stream_cursor_is_eof(lexer->streamCursor)) {
			kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_END_OF_FILE_PARSING_STRING);
			return;
		}
		if (!kar_stream_cursor_next(lexer->streamCursor)) {
			kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_STRING_END)) {
			next_token_default(lexer, KAR_LEXER_STATUS_UNKNOWN);
			return;
		}
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_STRING_ESCAPE)) {
			kar_token_add_str(lexer->current, kar_stream_cursor_get(lexer->streamCursor));
			if (!kar_stream_cursor_next(lexer->streamCursor)) {
				kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
			}
			kar_token_add_str(lexer->current, kar_stream_cursor_get(lexer->streamCursor));
		}
		kar_token_add_str(lexer->current, kar_stream_cursor_get(lexer->streamCursor));
	}
}

static void parse_singleline_comment(KarFirstLexer* lexer) {
	while (true) {
		if (kar_stream_cursor_is_eof(lexer->streamCursor)) {
			return;
		}
		if (!kar_stream_cursor_next(lexer->streamCursor)) {
			kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_SPACE_NEW_LINE)) {
			next_token_default(lexer, KAR_LEXER_STATUS_UNKNOWN);
			return;
		}
		kar_token_add_str(lexer->current, kar_stream_cursor_get(lexer->streamCursor));
	}
}

static void parse_multiline_comment(KarFirstLexer* lexer) {
	while (true) {
		if (kar_stream_cursor_is_eof(lexer->streamCursor)) {
			kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_END_OF_FILE_PARSING_COMMENT);
			return;
			
		}
		if (!kar_stream_cursor_next(lexer->streamCursor)) {
			kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_MULTILINE_COMMENT_END)) {
			next_token_default(lexer, KAR_LEXER_STATUS_UNKNOWN);
			return;
		}
		kar_token_add_str(lexer->current, kar_stream_cursor_get(lexer->streamCursor));
	}
}

bool kar_first_lexer_run(KarFirstLexer* lexer) {
	KarStreamCursor* streamCursor = lexer->streamCursor;
	while (!kar_stream_cursor_is_eof(streamCursor)) {
		if (!kar_stream_cursor_is_good(streamCursor)) {
			kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_READ_STREAM);
			return false;
		}
		if (!kar_stream_cursor_next(streamCursor)) {
			next_token(lexer, KAR_TOKEN_UNKNOWN, KAR_LEXER_STATUS_UNKNOWN);
			kar_token_set_str(lexer->current, kar_stream_cursor_get(streamCursor));
			kar_module_error_set(lexer->module, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (kar_stream_cursor_is_eof(streamCursor)) {
			break;
		}
		if (kar_stream_cursor_is_equal(streamCursor, KAR_KEYWORD_STRING_START)) {
			next_token(lexer, KAR_TOKEN_VAL_STRING, KAR_LEXER_STATUS_UNKNOWN);
			parse_string(lexer);
			continue;
		}
		if (kar_stream_cursor_is_equal(streamCursor, KAR_KEYWORD_COMMENT_START)) {
			next_token(lexer, KAR_TOKEN_COMMENT, KAR_LEXER_STATUS_UNKNOWN);
			parse_singleline_comment(lexer);
			continue;
		}
		if (kar_stream_cursor_is_equal(streamCursor, KAR_KEYWORD_MULTILINE_COMMENT_START)) {
			next_token(lexer, KAR_TOKEN_COMMENT, KAR_LEXER_STATUS_UNKNOWN);
			parse_multiline_comment(lexer);
			continue;
		}
		if (kar_stream_cursor_is_equal(streamCursor, KAR_KEYWORD_SPACE_NEW_LINE)) {
			add_new_line(lexer);
			continue;
		}
		if (lexer->status == KAR_LEXER_STATUS_INDENT) {
			KarLexerStatus st = get_status_by_symbol(lexer);
			if (st != KAR_LEXER_STATUS_SPACE) {
				next_token_default(lexer, st);
			}
		} else if (lexer->status == KAR_LEXER_STATUS_SPACE) {
			KarLexerStatus st = get_status_by_symbol(lexer);
			if (st != KAR_LEXER_STATUS_SPACE) {
				next_token_default(lexer, st);
			}
		} else if (lexer->status == KAR_LEXER_STATUS_IDENTIFIER) {
			KarLexerStatus st = get_status_by_symbol(lexer);
			if (st != KAR_LEXER_STATUS_IDENTIFIER) {
				next_token_default(lexer, st);
			}
		} else if (lexer->status == KAR_LEXER_STATUS_SIGN) {
			KarLexerStatus st = get_status_by_symbol(lexer);
			next_token_default(lexer, st);
		} else if (lexer->status == KAR_LEXER_STATUS_UNKNOWN) {
			if (is_space(lexer)) {
				next_token_default(lexer, KAR_LEXER_STATUS_SPACE);
			}
			if (is_sign(lexer)) {
				next_token_default(lexer, KAR_LEXER_STATUS_SIGN);
			}
		}

		kar_token_add_str(lexer->current, kar_stream_cursor_get(streamCursor));
	}
	push_token(lexer);
	return kar_module_error_get_count() == 0;
}
