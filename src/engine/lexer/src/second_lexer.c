/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "lexer/second_lexer.h"

#include <string.h>

static void retype_if_check(KarToken* token, KarTokenType checkType, const char* str, KarTokenType newType) {
	if (token->type == checkType && !strcmp(token->str, str)) {
		token->type = newType;
	}
}

static bool retype_all(KarToken* token) {
	// Значения переменных.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "пусто", KAR_TOKEN_VAL_NULL);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "да", KAR_TOKEN_VAL_TRUE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "нет", KAR_TOKEN_VAL_FALSE);
	// TODO: Написать про число.
		
	// Объявления переменных.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "константа", KAR_TOKEN_FIELD_CONST);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "поле", KAR_TOKEN_FIELD_VAR);
		
	// Типы переменных.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "буль", KAR_TOKEN_VAR_BOOL);
		
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "число", KAR_TOKEN_VAR_INTEGER64);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "число8", KAR_TOKEN_VAR_INTEGER8);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "число16", KAR_TOKEN_VAR_INTEGER16);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "число32", KAR_TOKEN_VAR_INTEGER32);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "число64", KAR_TOKEN_VAR_INTEGER64);
		
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "счётное", KAR_TOKEN_VAR_UNSIGNED64);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "счётное8", KAR_TOKEN_VAR_UNSIGNED8);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "счётное16", KAR_TOKEN_VAR_UNSIGNED16);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "счётное32", KAR_TOKEN_VAR_UNSIGNED32);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "счётное64", KAR_TOKEN_VAR_UNSIGNED64);
		
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "дробное", KAR_TOKEN_VAR_FLOAT64);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "дробное32", KAR_TOKEN_VAR_FLOAT32);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "дробное64", KAR_TOKEN_VAR_FLOAT64);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "дробное80", KAR_TOKEN_VAR_FLOAT80);

	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "строка", KAR_TOKEN_VAR_STRING);
		
	// Составные переменные.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "перечисление", KAR_TOKEN_ENUMERATION);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "структура", KAR_TOKEN_STRUCTURE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "объединение", KAR_TOKEN_UNION);
	
	// Знаки
	retype_if_check(token, KAR_TOKEN_SIGN, "?", KAR_TOKEN_SIGN_NULLABLE);
	retype_if_check(token, KAR_TOKEN_SIGN, "(", KAR_TOKEN_SIGN_OPEN_BRACES);
	retype_if_check(token, KAR_TOKEN_SIGN, ")", KAR_TOKEN_SIGN_CLOSE_BRACES);
	retype_if_check(token, KAR_TOKEN_SIGN, ".", KAR_TOKEN_SIGN_GET_FIELD);
	retype_if_check(token, KAR_TOKEN_SIGN, ",", KAR_TOKEN_SIGN_COMMA);
	retype_if_check(token, KAR_TOKEN_SIGN, ":", KAR_TOKEN_SIGN_COLON);

	retype_if_check(token, KAR_TOKEN_SIGN, "=", KAR_TOKEN_SIGN_ASSIGN);

	retype_if_check(token, KAR_TOKEN_SIGN, "+", KAR_TOKEN_SIGN_PLUS);
	retype_if_check(token, KAR_TOKEN_SIGN, "-", KAR_TOKEN_SIGN_MINUS);
	retype_if_check(token, KAR_TOKEN_SIGN, "*", KAR_TOKEN_SIGN_MUL);
	retype_if_check(token, KAR_TOKEN_SIGN, "/", KAR_TOKEN_SIGN_DIV);
	retype_if_check(token, KAR_TOKEN_SIGN, "%", KAR_TOKEN_SIGN_MOD);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "и", KAR_TOKEN_SIGN_AND);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "или", KAR_TOKEN_SIGN_OR);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "не", KAR_TOKEN_SIGN_NOT);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "равно", KAR_TOKEN_SIGN_EQUAL);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "больше", KAR_TOKEN_SIGN_GREATER);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "меньше", KAR_TOKEN_SIGN_LESS);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "битИ", KAR_TOKEN_SIGN_BIT_AND);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "битИли", KAR_TOKEN_SIGN_BIT_OR);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "битНе", KAR_TOKEN_SIGN_BIT_NOT);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "битИИли", KAR_TOKEN_SIGN_BIT_XOR);
	
	// Управление последовательностью выполенения.
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "блок", KAR_TOKEN_COMMAND_BLOCK);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "пустой", KAR_TOKEN_COMMAND_PASS);

	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "если", KAR_TOKEN_COMMAND_IF);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "то", KAR_TOKEN_COMMANT_THEN);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "иначе", KAR_TOKEN_COMMAND_ELSE);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "цикл", KAR_TOKEN_COMMAND_FOR);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "от", KAR_TOKEN_COMMAND_FROM);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "до", KAR_TOKEN_COMMAND_TO);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "шаг", KAR_TOKEN_COMMAND_STEP);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "пройти", KAR_TOKEN_COMMAND_FOREACH);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "взять", KAR_TOKEN_COMMAND_GET);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "пока", KAR_TOKEN_COMMAND_WHILE);

	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "пропустить", KAR_TOKEN_COMMAND_CONTINUE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "прервать", KAR_TOKEN_COMMAND_BREAK);
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "раскрыть", KAR_TOKEN_COMMAND_OPEN);

	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "функция", KAR_TOKEN_FUNCTION);

	//KAR_TOKEN_FUNC_PRIVATE,
	//KAR_TOKEN_FUNC_PROTECTED,
	//KAR_TOKEN_FUNC_PUBLIC,
	//KAR_TOKEN_FUNC_LOCAL,
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "стат", KAR_TOKEN_FUNC_STAT);

	//KAR_TOKEN_ARGUMENT_CONST,
	//KAR_TOKEN_ARGUMENT_VAR,
	//KAR_TOKEN_ARGUMENT_POINTER,
	
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "возвращает", KAR_TOKEN_FUNC_RETURN_TYPE);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "вернуть", KAR_TOKEN_FUNC_RETURN);

	// Класс

	// Шаблон класса

	// Тест
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "тест", KAR_TOKEN_TEST);
	retype_if_check(token, KAR_TOKEN_IDENTIFIER, "проверить", KAR_TOKEN_CHECK);
		
	return true;
}

bool kar_second_lexer_run(KarModule* module) {
	kar_token_foreach(module->token, retype_all);
	return true;
}
