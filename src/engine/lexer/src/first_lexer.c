/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/first_lexer.h"

#include <string.h>

#include "core/alloc.h"
#include "core/module_error.h"
#include "lexer/keyword.h"

const char* ERROR_READ_STREAM = "Не могу прочитать модуль. Нет доступа к модулю, он повреждён или удалён.";
const char* ERROR_SYMBOL_STRING = "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.";
const char* ERROR_UNKNOWN_SYMBOL = "Обнаружен неожиданный символ при разборе файла";
const char* ERROR_END_OF_FILE_PARSING_STRING = "Неожиданный конец файла при разборе строки. Возможно забыты закрывающие строку кавычки.";
const char* ERROR_END_OF_FILE_PARSING_COMMENT = "Неожиданный конец файла при разборе многострочного комментария. Возможно забыт символ \"*/\" для завершения комментария в конце файла.";

KarFirstLexer* kar_first_lexer_create(KarStream* stream, KarModule* module) {
	KAR_CREATE(lexer, KarFirstLexer);
	
	lexer->streamCursor = kar_stream_cursor_create(stream);
	
	lexer->current = kar_token_create();
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
static char* create_string_to_hex(const char* input)
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
	char* hex_code = create_string_to_hex(kar_stream_cursor_get(lexer->streamCursor));
	snprintf(buff, sizeof(buff), "%s: %s (код %s).", ERROR_UNKNOWN_SYMBOL, kar_stream_cursor_get(lexer->streamCursor), hex_code);
	KAR_FREE(hex_code);
	kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, buff);
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
		kar_token_child_add(lexer->module->token, lexer->current);
	} else {
		kar_token_free(lexer->current);
	}
	lexer->current = NULL;
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

static bool add_char_to_lexer(int32_t code, size_t count, KarFirstLexer* lexer) {
	if (count == 0 || count > 5) {
		kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, "Неверный номер символа кодировки Юникод.");
		return false;
	}

	char res[5];
	if (code < 0) {
		kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, "Неверный номер символа кодировки Юникод.");
		return false;
	} else if (code < 0x80) {
		res[0] = (char)code;
		res[1] = 0;
	} else if (code < 0x800) {
		res[0] = (char)((code >> 6) | 0xC0);
		res[1] = (char)((code & 0x3F) | 0x80);
		res[2] = 0;
	} else if (code < 0x10000) {
		res[0] = (char)((code >> 12) | 0xE0);
		res[1] = (char)(((code >> 6) & 0x3F) | 0x80);
		res[2] = (char)((code & 0x3F) | 0x80);
		res[3] = 0;
	} else if (code < 0x110000) {
		res[0] = (char)((code >> 18) | 0xF0);
		res[1] = (char)((code >> 12 & 0x3F) | 0x80);
		res[2] = (char)((code >> 6 & 0x3F) | 0x80);
		res[3] = (char)((code & 0x3F) | 0x80);
		res[4] = 0;
	} else {
		kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, "Неверный номер символа кодировки Юникод.");
		return false;
	}
	kar_token_add_str(lexer->current, res);
	return true;
}

static bool parse_hexadecimal_string(KarFirstLexer* lexer, bool *is_next_char) {
	int32_t code = 0;
	size_t count = 0;
	while(true) {
		if (!kar_stream_cursor_next(lexer->streamCursor)) {
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		unsigned char* current = (unsigned char*)lexer->streamCursor->currentChar;
		
		if (current[0] == ' ') {
			break;
		}
		if (current[0] == ',') {
			if (!add_char_to_lexer(code, count, lexer)) {
				return false;
			}
			code = 0;
			count = 0;
			continue;
		}
		
		if (current[0] >= 0x30 && current[0] <= 0x39) {
			code = (code << 4) + (current[0] - 0x30);
		} else if (current[0] == 0xD0 && (current[1] >= 0x90 && current[1] <= 0x95)) {
			code = (code << 4) + (current[1] - 0x90 + 10);
		} else if (current[0] == 0xD0 && (current[1] >= 0xB0 && current[1] <= 0xB5)) {
			code = (code << 4) + (current[1] - 0xB0 + 10);
		} else {
			*is_next_char = false;
			break;
		}
		count++;
	}
	if (!add_char_to_lexer(code, count, lexer)) {
		return false;
	}
	return true;
}

static void parse_string(KarFirstLexer* lexer) {
	bool is_next_char = true;
	while (true) {
		if (kar_stream_cursor_is_eof(lexer->streamCursor)) {
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_END_OF_FILE_PARSING_STRING);
			return;
		}
		if (is_next_char && !kar_stream_cursor_next(lexer->streamCursor)) {
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		is_next_char = true;
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_STRING_END)) {
			next_token_default(lexer, KAR_LEXER_STATUS_UNKNOWN);
			return;
		}
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_STRING_ESCAPE)) {
			if (!kar_stream_cursor_next(lexer->streamCursor)) {
				kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
			}
			if (!strcmp(lexer->streamCursor->currentChar, "н")) {
				kar_token_add_str(lexer->current, "\n");
			} else if (!strcmp(lexer->streamCursor->currentChar, "к")) {
				kar_token_add_str(lexer->current, "\r");
			} else if (!strcmp(lexer->streamCursor->currentChar, "т")) {
				kar_token_add_str(lexer->current, "\t");
			} else if (!strcmp(lexer->streamCursor->currentChar, "\"")) {
				kar_token_add_str(lexer->current, "\"");
			} else if (!strcmp(lexer->streamCursor->currentChar, "\\")) {
				kar_token_add_str(lexer->current, "\\");
			} else if (!strcmp(lexer->streamCursor->currentChar, "ш")) {
				if (!parse_hexadecimal_string(lexer, &is_next_char)) {
					return;
				}
			} else {
				char buff[1024];
				snprintf(buff, sizeof(buff), "Неверный управляющий символ: \\%s.", lexer->streamCursor->currentChar);
				kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, buff);
			}
			continue;
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
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_SPACE_NEW_LINE)) {
			next_token_default(lexer, KAR_LEXER_STATUS_UNKNOWN);
			return;
		}
		kar_token_add_str(lexer->current, kar_stream_cursor_get(lexer->streamCursor));
	}
}

static void parse_multiline_comment(KarFirstLexer* lexer) {
	bool end_mul = false;
	while (true) {
		if (kar_stream_cursor_is_eof(lexer->streamCursor)) {
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_END_OF_FILE_PARSING_COMMENT);
			return;
			
		}
		if (!kar_stream_cursor_next(lexer->streamCursor)) {
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
		if (end_mul && kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_SIGN_DIV)) {
			lexer->current->str[strlen(lexer->current->str) - 1] = 0;
			return;
		}
		char* cur_char = kar_stream_cursor_get(lexer->streamCursor);
		end_mul = !strcmp(cur_char, KAR_KEYWORD_SIGN_MUL);
		kar_token_add_str(lexer->current, cur_char);
	}
}

static void add_string_to_token(KarToken* token, KarStreamCursor* streamCursor) {
	char* added = kar_stream_cursor_get(streamCursor);
	if (!strcmp(added, KAR_KEYWORD_SPACE_CARRIAGE_RETURN)) {
		return;
	}
	kar_token_add_str(token, added);
}

bool kar_first_lexer_run(KarFirstLexer* lexer) {
	KarStreamCursor* streamCursor = lexer->streamCursor;
	if (!kar_stream_cursor_is_eof(streamCursor) && kar_stream_cursor_is_good(streamCursor)) {
		if (!kar_stream_cursor_next(streamCursor)) {
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		} else if (kar_stream_cursor_is_equal(streamCursor, "\xEF\xBB\xBF")) {
			kar_cursor_init(&streamCursor->cursor);
			if (!kar_stream_cursor_next(streamCursor)) {
				kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
			}
		}
	}
	while (!kar_stream_cursor_is_eof(streamCursor)) {
		if (!kar_stream_cursor_is_good(streamCursor)) {
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_READ_STREAM);
			return false;
		}
		if (kar_stream_cursor_is_equal(streamCursor, KAR_KEYWORD_STRING_START)) {
			next_token(lexer, KAR_TOKEN_VAL_STRING, KAR_LEXER_STATUS_UNKNOWN);
			parse_string(lexer);
		} else if (kar_stream_cursor_is_equal(streamCursor, KAR_KEYWORD_SPACE_NEW_LINE)) {
			add_new_line(lexer);
		} else {
			if (lexer->status == KAR_LEXER_STATUS_INDENT) {
				KarLexerStatus st = get_status_by_symbol(lexer);
				if (st != KAR_LEXER_STATUS_SPACE) {
					next_token_default(lexer, st);
				}
				add_string_to_token(lexer->current, streamCursor);
			} else if (lexer->status == KAR_LEXER_STATUS_SPACE) {
				KarLexerStatus st = get_status_by_symbol(lexer);
				if (st != KAR_LEXER_STATUS_SPACE) {
					next_token_default(lexer, st);
				}
				add_string_to_token(lexer->current, streamCursor);
			} else if (lexer->status == KAR_LEXER_STATUS_IDENTIFIER) {
				KarLexerStatus st = get_status_by_symbol(lexer);
				if (st != KAR_LEXER_STATUS_IDENTIFIER) {
					next_token_default(lexer, st);
				}
				add_string_to_token(lexer->current, streamCursor);
			} else if (lexer->status == KAR_LEXER_STATUS_SIGN) {
				if (!strcmp(lexer->current->str, KAR_KEYWORD_SIGN_DIV) && !strcmp(streamCursor->currentChar, KAR_KEYWORD_SIGN_DIV)) {
					KarCursor cursor = lexer->current->cursor;
					new_token(lexer, KAR_TOKEN_COMMENT);
					lexer->current->cursor = cursor;
					parse_singleline_comment(lexer);
					continue;
				} else if (!strcmp(lexer->current->str, KAR_KEYWORD_SIGN_DIV) && !strcmp(streamCursor->currentChar, KAR_KEYWORD_SIGN_MUL)) {
					KarCursor cursor = lexer->current->cursor;
					new_token(lexer, KAR_TOKEN_COMMENT);
					lexer->current->cursor = cursor;
					parse_multiline_comment(lexer);
				} else {
					KarLexerStatus st = get_status_by_symbol(lexer);
					next_token_default(lexer, st);
					add_string_to_token(lexer->current, streamCursor);
				}
			} else if (lexer->status == KAR_LEXER_STATUS_UNKNOWN) {
				if (is_space(lexer)) {
					next_token_default(lexer, KAR_LEXER_STATUS_SPACE);
				}
				if (is_sign(lexer)) {
					next_token_default(lexer, KAR_LEXER_STATUS_SIGN);
				}
				add_string_to_token(lexer->current, streamCursor);
			}
		}
		
		if (!kar_stream_cursor_next(streamCursor)) {
			next_token(lexer, KAR_TOKEN_UNKNOWN, KAR_LEXER_STATUS_UNKNOWN);
			add_string_to_token(lexer->current, streamCursor);
			kar_module_error_create_add(&lexer->module->errors, &lexer->streamCursor->cursor, 1, ERROR_SYMBOL_STRING);
		}
	}
	push_token(lexer);
	return kar_module_error_get_count(lexer->module) == 0;
}
