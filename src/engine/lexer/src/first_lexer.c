/* Copyright © 2020,2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021,2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/first_lexer.h"

#include <string.h>
#include <stdint.h>

#include "core/alloc.h"
#include "core/string.h"
#include "model/project_error.h"
#include "lexer/keyword.h"
#include "lexer/check_alphabet.h"

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
static void set_error(KarFirstLexer* lexer, int code, const KarString* description) {
	kar_project_error_list_create_add(lexer->module->errors, &lexer->streamCursor->cursor, code, description);
}

static const KarString* SPACES[] = {
	KAR_KEYWORD_SPACE,
	KAR_KEYWORD_SPACE_TAB,
	KAR_KEYWORD_SPACE_CARRIAGE_RETURN
};
static const size_t SPACE_COUNT = sizeof(SPACES) / sizeof(KarString*);

static const KarString* SIGNS[] = {
	KAR_KEYWORD_SIGN_OPEN_BRACES,
	KAR_KEYWORD_SIGN_CLOSE_BRACES,
	KAR_KEYWORD_SIGN_UNCLEAN,
	KAR_KEYWORD_SIGN_CLEAN,
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
static const size_t SIGN_COUNT = sizeof(SIGNS) / sizeof(KarString*);

static bool is_space(KarFirstLexer* lexer) {
	return kar_stream_cursor_is_one_of(lexer->streamCursor, SPACES, SPACE_COUNT);
}

static bool is_identifier(KarFirstLexer* lexer) {
	const KarString* str = kar_stream_cursor_get(lexer->streamCursor);
	// TODO: Нужно проверить 1 символ, а вызываемая функция проверяет целую строку. 
	return kar_check_identifiers_alphabet(str);
}

static bool is_sign(KarFirstLexer* lexer) {
	return kar_stream_cursor_is_one_of(lexer->streamCursor, SIGNS, SIGN_COUNT);
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
	KarString* hex_code = kar_string_encode_hex(kar_stream_cursor_get(lexer->streamCursor));
	KarString* error_string = kar_string_create_format("Обнаружен неожиданный символ при разборе файла: %s (код %s).", kar_stream_cursor_get(lexer->streamCursor), hex_code);
	kar_string_free(hex_code);
	set_error(lexer, 1, error_string);
	kar_string_free(error_string);
	return KAR_LEXER_STATUS_UNKNOWN;
}

// ------------------------------
static void new_token(KarFirstLexer* lexer, KarTokenType type) {
	lexer->current = kar_token_create();
	lexer->current->type = type;
	lexer->current->cursor = lexer->streamCursor->cursor;
}

static void push_token(KarFirstLexer* lexer) {
	if (lexer->current->str != NULL || lexer->current->type != KAR_TOKEN_UNKNOWN) {
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
	kar_token_set_str(lexer->current, "");
	lexer->current->cursor = lexer->streamCursor->cursor;
	lexer->status = KAR_LEXER_STATUS_INDENT;
}

// TODO: Возможно часть логики надо перенести в unicode.c
static bool add_char_to_lexer(int32_t code, size_t count, KarFirstLexer* lexer) {
	if (count == 0 || count > 5) {
		set_error(lexer, 1, "Неверный номер символа кодировки Юникод.");
		return false;
	}

	KarString res[5];
	if (code < 0) {
		set_error(lexer, 1, "Неверный номер символа кодировки Юникод.");
		return false;
	} else if (code < 0x80) {
		res[0] = (KarString)code;
		res[1] = 0;
	} else if (code < 0x800) {
		res[0] = (KarString)((code >> 6) | 0xC0);
		res[1] = (KarString)((code & 0x3F) | 0x80);
		res[2] = 0;
	} else if (code < 0x10000) {
		res[0] = (KarString)((code >> 12) | 0xE0);
		res[1] = (KarString)(((code >> 6) & 0x3F) | 0x80);
		res[2] = (KarString)((code & 0x3F) | 0x80);
		res[3] = 0;
	} else if (code < 0x110000) {
		res[0] = (KarString)((code >> 18) | 0xF0);
		res[1] = (KarString)((code >> 12 & 0x3F) | 0x80);
		res[2] = (KarString)((code >> 6 & 0x3F) | 0x80);
		res[3] = (KarString)((code & 0x3F) | 0x80);
		res[4] = 0;
	} else {
		set_error(lexer, 1, "Неверный номер символа кодировки Юникод.");
		return false;
	}
	kar_token_add_str(lexer->current, res);
	return true;
}

// TODO: Возможно часть логики надо перенести в unicode.c
static bool parse_hexadecimal_string(KarFirstLexer* lexer, bool *is_next_char) {
	int32_t code = 0;
	size_t count = 0;
	while(true) {
		if (!kar_stream_cursor_next(lexer->streamCursor)) {
			set_error(lexer, 1, "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.");
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
	return add_char_to_lexer(code, count, lexer);
}

static void parse_string(KarFirstLexer* lexer) {
	bool is_next_char = true;
	kar_token_set_str(lexer->current, "");
	while (true) {
		if (kar_stream_cursor_is_eof(lexer->streamCursor)) {
			set_error(lexer, 1, "Неожиданный конец файла при разборе строки. Возможно забыты закрывающие строку кавычки.");
			return;
		}
		if (is_next_char && !kar_stream_cursor_next(lexer->streamCursor)) {
			set_error(lexer, 1, "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.");
		}
		is_next_char = true;
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_STRING_END)) {
			next_token_default(lexer, KAR_LEXER_STATUS_UNKNOWN);
			return;
		}
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_STRING_ESCAPE)) {
			if (!kar_stream_cursor_next(lexer->streamCursor)) {
				set_error(lexer, 1, "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.");
			}
			if (kar_string_equal(lexer->streamCursor->currentChar, "н")) {
				kar_token_add_str(lexer->current, "\n");
			} else if (kar_string_equal(lexer->streamCursor->currentChar, "к")) {
				kar_token_add_str(lexer->current, "\r");
			} else if (kar_string_equal(lexer->streamCursor->currentChar, "т")) {
				kar_token_add_str(lexer->current, "\t");
			} else if (kar_string_equal(lexer->streamCursor->currentChar, "\"")) {
				kar_token_add_str(lexer->current, "\"");
			} else if (kar_string_equal(lexer->streamCursor->currentChar, "\\")) {
				kar_token_add_str(lexer->current, "\\");
			} else if (kar_string_equal(lexer->streamCursor->currentChar, "ш")) {
				if (!parse_hexadecimal_string(lexer, &is_next_char)) {
					return;
				}
			} else {
				KarString* error_string = kar_string_create_format("Неверный управляющий символ: \\%s.", lexer->streamCursor->currentChar);
				set_error(lexer, 1, error_string);
				kar_string_free(error_string);
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
			set_error(lexer, 1, "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.");
		}
		if (kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_SPACE_NEW_LINE)) {
			next_token_default(lexer, KAR_LEXER_STATUS_UNKNOWN);
			return;
		}
	}
}

static void parse_multiline_comment(KarFirstLexer* lexer) {
	bool end_mul = false;
	while (true) {
		if (kar_stream_cursor_is_eof(lexer->streamCursor)) {
			set_error(lexer, 1, "Неожиданный конец файла при разборе многострочного комментария. Возможно забыт токен завершения комментария \"*/\" в конце файла.");
			return;
			
		}
		if (!kar_stream_cursor_next(lexer->streamCursor)) {
			set_error(lexer, 1, "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.");
		}
		if (end_mul && kar_stream_cursor_is_equal(lexer->streamCursor, KAR_KEYWORD_SIGN_DIV)) {
			return;
		}
		KarString* cur_char = kar_stream_cursor_get(lexer->streamCursor);
		end_mul = kar_string_equal(cur_char, KAR_KEYWORD_SIGN_MUL);
	}
}

static void add_string_to_token(KarToken* token, KarStreamCursor* streamCursor) {
	KarString* added = kar_stream_cursor_get(streamCursor);
	if (kar_string_equal(added, KAR_KEYWORD_SPACE_CARRIAGE_RETURN)) {
		return;
	}
	kar_token_add_str(token, added);
}

bool kar_first_lexer_run(KarFirstLexer* lexer) {
	KarStreamCursor* streamCursor = lexer->streamCursor;
	if (!kar_stream_cursor_is_eof(streamCursor) && kar_stream_cursor_is_good(streamCursor)) {
		if (!kar_stream_cursor_next(streamCursor)) {
			set_error(lexer, 1, "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.");
		} else if (kar_stream_cursor_is_equal(streamCursor, "\xEF\xBB\xBF")) {
			kar_cursor_init(&streamCursor->cursor);
			if (!kar_stream_cursor_next(streamCursor)) {
				set_error(lexer, 1, "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.");
			}
		}
	}
	while (!kar_stream_cursor_is_eof(streamCursor)) {
		if (!kar_stream_cursor_is_good(streamCursor)) {
			set_error(lexer, 1, "Не могу прочитать модуль. Нет доступа к модулю, он повреждён или удалён.");
			return false;
		}
		if (kar_stream_cursor_is_equal(streamCursor, KAR_KEYWORD_STRING_START)) {
			next_token(lexer, KAR_TOKEN_VAL_STRING, KAR_LEXER_STATUS_UNKNOWN);
			parse_string(lexer);
		} else if (kar_stream_cursor_is_equal(streamCursor, KAR_KEYWORD_SPACE_CARRIAGE_RETURN)) {
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
					kar_token_set_str(lexer->current, NULL);
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
				if (
					lexer->current->str &&
					kar_string_equal(lexer->current->str, KAR_KEYWORD_SIGN_DIV) &&
					kar_string_equal(streamCursor->currentChar, KAR_KEYWORD_SIGN_DIV)
				) {
					KarCursor cursor = lexer->current->cursor;
					new_token(lexer, KAR_TOKEN_COMMENT);
					lexer->current->cursor = cursor;
					parse_singleline_comment(lexer);
					continue;
				} else if (
					lexer->current->str &&
					kar_string_equal(lexer->current->str, KAR_KEYWORD_SIGN_DIV) &&
					kar_string_equal(streamCursor->currentChar, KAR_KEYWORD_SIGN_MUL)
				) {
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
			set_error(lexer, 1, "Ошибочный символ. Возможно файл повреждён или сохранён не в кодировке UTF-8.");
		}
	}
	push_token(lexer);
	return kar_project_error_list_count(lexer->module->errors) == 0;
}
