/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_KEYWORD_H
#define KAR_KEYWORD_H

// Пробельные значения.
#define KAR_KEYWORD_SPACE " "
#define KAR_KEYWORD_SPACE_TAB "\t"
#define KAR_KEYWORD_SPACE_CARRIAGE_RETURN "\r"
#define KAR_KEYWORD_SPACE_NEW_LINE "\n"

// Комментарии
#define KAR_KEYWORD_COMMENT_START "//"
#define KAR_KEYWORD_MULTILINE_COMMENT_START "/*"
#define KAR_KEYWORD_MULTILINE_COMMENT_END "*/"

// Строки
#define KAR_KEYWORD_STRING_START "\""
#define KAR_KEYWORD_STRING_END "\""
#define KAR_KEYWORD_STRING_ESCAPE "\\"

// Значения переменных.
#define KAR_KEYWORD_VAL_NULL "пусто"
#define KAR_KEYWORD_VAL_TRUE "да"
#define KAR_KEYWORD_VAL_FALSE "нет"

// Объявления переменных.
#define KAR_KEYWORD_DECLARATION_CONST "константа"
#define KAR_KEYWORD_DECLARATION_VAR "поле"

#define KAR_KEYWORD_VAR_MODIFIER_DYNAMIC "динамическое"
#define KAR_KEYWORD_VAR_MODIFIER_PRIVATE "закрытое"
#define KAR_KEYWORD_VAR_MODIFIER_PROTECTED "защищённое"
#define KAR_KEYWORD_VAR_MODIFIER_PUBLIC "открытое"

// Типы переменных.
#define KAR_KEYWORD_TYPE_BOOL "буль"

#define KAR_KEYWORD_TYPE_INTEGER "число"
#define KAR_KEYWORD_TYPE_INTEGER8 "число8"
#define KAR_KEYWORD_TYPE_INTEGER16 "число16"
#define KAR_KEYWORD_TYPE_INTEGER32 "число32"
#define KAR_KEYWORD_TYPE_INTEGER64 "число64"

#define KAR_KEYWORD_TYPE_UNSIGNED "счётное"
#define KAR_KEYWORD_TYPE_UNSIGNED8 "счётное8"
#define KAR_KEYWORD_TYPE_UNSIGNED16 "счётное16"
#define KAR_KEYWORD_TYPE_UNSIGNED32 "счётное32"
#define KAR_KEYWORD_TYPE_UNSIGNED64 "счётное64"

#define KAR_KEYWORD_TYPE_FLOAT "дробное"
#define KAR_KEYWORD_TYPE_FLOAT32 "дробное32"
#define KAR_KEYWORD_TYPE_FLOAT64 "дробное64"
#define KAR_KEYWORD_TYPE_FLOAT80 "дробное80"

#define KAR_KEYWORD_TYPE_STRING "строка"

// Составные переменные.
#define KAR_KEYWORD_TYPE_ENUMERATION "перечисление"
#define KAR_KEYWORD_TYPE_STRUCTURE "структура"
#define KAR_KEYWORD_TYPE_UNION "объединение"

// Знаки
#define KAR_KEYWORD_SIGN_OPEN_BRACES "("
#define KAR_KEYWORD_SIGN_CLOSE_BRACES ")"
#define KAR_KEYWORD_SIGN_NULLABLE "?"
#define KAR_KEYWORD_SIGN_GET_FIELD "."
#define KAR_KEYWORD_SIGN_COMMA ","
#define KAR_KEYWORD_SIGN_COLON ":"

#define KAR_KEYWORD_SIGN_ASSIGN "="

#define KAR_KEYWORD_SIGN_PLUS "+"
#define KAR_KEYWORD_SIGN_MINUS "-"
#define KAR_KEYWORD_SIGN_MUL "*"
#define KAR_KEYWORD_SIGN_DIV "/"
#define KAR_KEYWORD_SIGN_MOD "%"

#define KAR_KEYWORD_SIGN_AND "и"
#define KAR_KEYWORD_SIGN_OR "или"
#define KAR_KEYWORD_SIGN_NOT "не"
#define KAR_KEYWORD_SIGN_EQUAL "равно"
#define KAR_KEYWORD_SIGN_GREATER "больше"
#define KAR_KEYWORD_SIGN_LESS "меньше"

#define KAR_KEYWORD_SIGN_BIT_AND "битИ"
#define KAR_KEYWORD_SIGN_BIT_OR "битИли"
#define KAR_KEYWORD_SIGN_BIT_NOT "битНе"
#define KAR_KEYWORD_SIGN_BIT_XOR "битИИли"

// Управление последовательностью выполенения.
#define KAR_KEYWORD_COMMAND_BLOCK "блок"
#define KAR_KEYWORD_COMMAND_PASS "пустой"

#define KAR_KEYWORD_COMMAND_IF "если"
#define KAR_KEYWORD_COMMANT_THEN "то"
#define KAR_KEYWORD_COMMAND_ELSE "иначе"

#define KAR_KEYWORD_COMMAND_FOR "цикл"
#define KAR_KEYWORD_COMMAND_FROM "от"
#define KAR_KEYWORD_COMMAND_TO "до"
#define KAR_KEYWORD_COMMAND_STEP "шаг"

#define KAR_KEYWORD_COMMAND_FOREACH "пройти"
#define KAR_KEYWORD_COMMAND_GET "взять"

#define KAR_KEYWORD_COMMAND_WHILE "пока"

#define KAR_KEYWORD_COMMAND_CONTINUE "пропустить"
#define KAR_KEYWORD_COMMAND_BREAK "прервать"

#define KAR_KEYWORD_COMMAND_OPEN "раскрыть"

#define KAR_KEYWORD_DECLARATION_METHOD "метод"

//KAR_TOKEN_METHOD_PRIVATE,
//KAR_TOKEN_METHOD_PROTECTED,
//KAR_TOKEN_METHOD_PUBLIC,
//KAR_TOKEN_METHOD_LOCAL,
#define KAR_KEYWORD_STAT "стат"

//KAR_TOKEN_METHOD_ARGUMENT_CONST,
//KAR_TOKEN_METHOD_ARGUMENT_VAR,
//KAR_TOKEN_METHOD_ARGUMENT_POINTER,

#define KAR_KEYWORD_METHOD_RETURN "вернуть"

// Класс

// Шаблон класса

// Тест
#define KAR_KEYWORD_TEST "тест"
#define KAR_KEYWORD_TEST_CHECK "проверить"

#endif // KAR_KEYWORD_H
