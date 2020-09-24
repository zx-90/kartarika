/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/second_lexer.h"

#include <string.h>

static void retype_if_check(KToken* token, KTokenType checkType, const char* str, KTokenType newType) {
	if (token->type == checkType && !strcmp(token->str, str)) {
		token->type = newType;
	}
}

static bool retype_all(KToken* token) {
	// Значения переменных.
	retype_if_check(token, TOKEN_IDENTIFIER, "пусто", TOKEN_VAL_NULL);
	retype_if_check(token, TOKEN_IDENTIFIER, "да", TOKEN_VAL_TRUE);
	retype_if_check(token, TOKEN_IDENTIFIER, "нет", TOKEN_VAL_FALSE);
	// TODO: Написать про число.
		
	// Объявления переменных.
	retype_if_check(token, TOKEN_IDENTIFIER, "константа", TOKEN_FIELD_CONST);
	retype_if_check(token, TOKEN_IDENTIFIER, "поле", TOKEN_FIELD_VAR);
		
	// Типы переменных.
	retype_if_check(token, TOKEN_IDENTIFIER, "буль", TOKEN_VAR_BOOL);
		
	retype_if_check(token, TOKEN_IDENTIFIER, "число", TOKEN_VAR_INTEGER64);
	retype_if_check(token, TOKEN_IDENTIFIER, "число8", TOKEN_VAR_INTEGER8);
	retype_if_check(token, TOKEN_IDENTIFIER, "число16", TOKEN_VAR_INTEGER16);
	retype_if_check(token, TOKEN_IDENTIFIER, "число32", TOKEN_VAR_INTEGER32);
	retype_if_check(token, TOKEN_IDENTIFIER, "число64", TOKEN_VAR_INTEGER64);
		
	retype_if_check(token, TOKEN_IDENTIFIER, "счётное", TOKEN_VAR_UNSIGNED64);
	retype_if_check(token, TOKEN_IDENTIFIER, "счётное8", TOKEN_VAR_UNSIGNED8);
	retype_if_check(token, TOKEN_IDENTIFIER, "счётное16", TOKEN_VAR_UNSIGNED16);
	retype_if_check(token, TOKEN_IDENTIFIER, "счётное32", TOKEN_VAR_UNSIGNED32);
	retype_if_check(token, TOKEN_IDENTIFIER, "счётное64", TOKEN_VAR_UNSIGNED64);
		
	retype_if_check(token, TOKEN_IDENTIFIER, "дробное", TOKEN_VAR_FLOAT64);
	retype_if_check(token, TOKEN_IDENTIFIER, "дробное32", TOKEN_VAR_FLOAT32);
	retype_if_check(token, TOKEN_IDENTIFIER, "дробное64", TOKEN_VAR_FLOAT64);
	retype_if_check(token, TOKEN_IDENTIFIER, "дробное80", TOKEN_VAR_FLOAT80);

	retype_if_check(token, TOKEN_IDENTIFIER, "строка", TOKEN_VAR_STRING);
		
	// Составные переменные.
	retype_if_check(token, TOKEN_IDENTIFIER, "перечисление", TOKEN_ENUMERATION);
	retype_if_check(token, TOKEN_IDENTIFIER, "структура", TOKEN_STRUCTURE);
	retype_if_check(token, TOKEN_IDENTIFIER, "объединение", TOKEN_UNION);
	
	// Знаки
	retype_if_check(token, TOKEN_SIGN, "?", TOKEN_SIGN_NULLABLE);
	retype_if_check(token, TOKEN_SIGN, "(", TOKEN_SIGN_OPEN_BRACES);
	retype_if_check(token, TOKEN_SIGN, ")", TOKEN_SIGN_CLOSE_BRACES);
	retype_if_check(token, TOKEN_SIGN, ".", TOKEN_SIGN_GET_FIELD);
	retype_if_check(token, TOKEN_SIGN, ",", TOKEN_SIGN_COMMA);
	retype_if_check(token, TOKEN_SIGN, ":", TOKEN_SIGN_COLON);

	retype_if_check(token, TOKEN_SIGN, "=", TOKEN_SIGN_ASSIGN);

	retype_if_check(token, TOKEN_SIGN, "+", TOKEN_SIGN_PLUS);
	retype_if_check(token, TOKEN_SIGN, "-", TOKEN_SIGN_MINUS);
	retype_if_check(token, TOKEN_SIGN, "*", TOKEN_SIGN_MUL);
	retype_if_check(token, TOKEN_SIGN, "/", TOKEN_SIGN_DIV);
	retype_if_check(token, TOKEN_SIGN, "%", TOKEN_SIGN_MOD);
	
	retype_if_check(token, TOKEN_IDENTIFIER, "и", TOKEN_SIGN_AND);
	retype_if_check(token, TOKEN_IDENTIFIER, "или", TOKEN_SIGN_OR);
	retype_if_check(token, TOKEN_IDENTIFIER, "не", TOKEN_SIGN_NOT);
	retype_if_check(token, TOKEN_IDENTIFIER, "равно", TOKEN_SIGN_EQUAL);
	retype_if_check(token, TOKEN_IDENTIFIER, "больше", TOKEN_SIGN_GREATER);
	retype_if_check(token, TOKEN_IDENTIFIER, "меньше", TOKEN_SIGN_LESS);
	
	retype_if_check(token, TOKEN_IDENTIFIER, "битИ", TOKEN_SIGN_BIT_AND);
	retype_if_check(token, TOKEN_IDENTIFIER, "битИли", TOKEN_SIGN_BIT_OR);
	retype_if_check(token, TOKEN_IDENTIFIER, "битНе", TOKEN_SIGN_BIT_NOT);
	retype_if_check(token, TOKEN_IDENTIFIER, "битИИли", TOKEN_SIGN_BIT_XOR);
	
	// Управление последовательностью выполенения.
	retype_if_check(token, TOKEN_IDENTIFIER, "блок", TOKEN_COMMAND_BLOCK);
	retype_if_check(token, TOKEN_IDENTIFIER, "пустой", TOKEN_COMMAND_PASS);

	
	retype_if_check(token, TOKEN_IDENTIFIER, "если", TOKEN_COMMAND_IF);
	retype_if_check(token, TOKEN_IDENTIFIER, "то", TOKEN_COMMANT_THEN);
	retype_if_check(token, TOKEN_IDENTIFIER, "иначе", TOKEN_COMMAND_ELSE);
	
	retype_if_check(token, TOKEN_IDENTIFIER, "цикл", TOKEN_COMMAND_FOR);
	retype_if_check(token, TOKEN_IDENTIFIER, "от", TOKEN_COMMAND_FROM);
	retype_if_check(token, TOKEN_IDENTIFIER, "до", TOKEN_COMMAND_TO);
	retype_if_check(token, TOKEN_IDENTIFIER, "шаг", TOKEN_COMMAND_STEP);
	
	retype_if_check(token, TOKEN_IDENTIFIER, "пройти", TOKEN_COMMAND_FOREACH);
	retype_if_check(token, TOKEN_IDENTIFIER, "взять", TOKEN_COMMAND_GET);
	
	retype_if_check(token, TOKEN_IDENTIFIER, "пока", TOKEN_COMMAND_WHILE);

	retype_if_check(token, TOKEN_IDENTIFIER, "пропустить", TOKEN_COMMAND_CONTINUE);
	retype_if_check(token, TOKEN_IDENTIFIER, "прервать", TOKEN_COMMAND_BREAK);
	
	retype_if_check(token, TOKEN_IDENTIFIER, "раскрыть", TOKEN_COMMAND_OPEN);

	retype_if_check(token, TOKEN_IDENTIFIER, "функция", TOKEN_FUNCTION);

	//TOKEN_FUNC_PRIVATE,
	//TOKEN_FUNC_PROTECTED,
	//TOKEN_FUNC_PUBLIC,
	//TOKEN_FUNC_LOCAL,
	retype_if_check(token, TOKEN_IDENTIFIER, "стат", TOKEN_FUNC_STAT);

	//TOKEN_ARGUMENT_CONST,
	//TOKEN_ARGUMENT_VAR,
	//TOKEN_ARGUMENT_POINTER,
	
	retype_if_check(token, TOKEN_IDENTIFIER, "возвращает", TOKEN_FUNC_RETURN_TYPE);
	retype_if_check(token, TOKEN_IDENTIFIER, "вернуть", TOKEN_FUNC_RETURN);

	// Класс

	// Шаблон класса

	// Тест
	retype_if_check(token, TOKEN_IDENTIFIER, "тест", TOKEN_TEST);
	retype_if_check(token, TOKEN_IDENTIFIER, "проверить", TOKEN_CHECK);
		
	return true;
}

bool k_second_lexer_run(KModule* module) {
	k_token_foreach(module->token, retype_all);
	return true;
}
