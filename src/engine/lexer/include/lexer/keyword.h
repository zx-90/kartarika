/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2022 Abdullin Timur <abdtimurrif@gmail.com>
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
#define KAR_KEYWORD_VAL_NULL "Пусто"
#define KAR_KEYWORD_VAL_TRUE "Да"
#define KAR_KEYWORD_VAL_FALSE "Нет"
#define KAR_KEYWORD_VAL_NAN "НеЧисло"
#define KAR_KEYWORD_VAL_INFINITY "Бесконечность"
#define KAR_KEYWORD_VAL_PLUS_INFINITY "ПлюсБесконечность"
#define KAR_KEYWORD_VAL_MINUS_INFINITY "МинусБесконечность"

// Объявления переменных.
#define KAR_KEYWORD_DECLARATION_CONST "конст"
#define KAR_KEYWORD_DECLARATION_VAR "поле"

#define KAR_KEYWORD_VAR_MODIFIER_DYNAMIC "динамическое"
#define KAR_KEYWORD_VAR_MODIFIER_PRIVATE "закрытое"
#define KAR_KEYWORD_VAR_MODIFIER_PROTECTED "защищённое"
#define KAR_KEYWORD_VAR_MODIFIER_PUBLIC "открытое"

// Типы переменных.
#define KAR_KEYWORD_TYPE_BOOL "Буль"

#define KAR_KEYWORD_TYPE_INTEGER "Целое"
#define KAR_KEYWORD_TYPE_INTEGER8 "Целое8"
#define KAR_KEYWORD_TYPE_INTEGER16 "Целое16"
#define KAR_KEYWORD_TYPE_INTEGER32 "Целое32"
#define KAR_KEYWORD_TYPE_INTEGER64 "Целое64"

#define KAR_KEYWORD_TYPE_UNSIGNED "Счётное"
#define KAR_KEYWORD_TYPE_UNSIGNED8 "Счётное8"
#define KAR_KEYWORD_TYPE_UNSIGNED16 "Счётное16"
#define KAR_KEYWORD_TYPE_UNSIGNED32 "Счётное32"
#define KAR_KEYWORD_TYPE_UNSIGNED64 "Счётное64"

#define KAR_KEYWORD_TYPE_FLOAT "Дробное"
#define KAR_KEYWORD_TYPE_FLOAT32 "Дробное32"
#define KAR_KEYWORD_TYPE_FLOAT64 "Дробное64"
#define KAR_KEYWORD_TYPE_FLOAT80 "Дробное80"

#define KAR_KEYWORD_TYPE_STRING "Строка"

// Знаки
#define KAR_KEYWORD_SIGN_OPEN_BRACES "("
#define KAR_KEYWORD_SIGN_CLOSE_BRACES ")"
#define KAR_KEYWORD_SIGN_UNCLEAN "?"
#define KAR_KEYWORD_SIGN_CLEAN "!"
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
#define KAR_KEYWORD_SIGN_GREATER "бш"
#define KAR_KEYWORD_SIGN_GREATER_OR_EQUAL "бир"
#define KAR_KEYWORD_SIGN_LESS "мш"
#define KAR_KEYWORD_SIGN_LESS_OR_EQUAL "мир"

#define KAR_KEYWORD_SIGN_BIT_AND "би"
#define KAR_KEYWORD_SIGN_BIT_OR "били"
#define KAR_KEYWORD_SIGN_BIT_NOT "бне"
#define KAR_KEYWORD_SIGN_BIT_XOR "билине"
#define KAR_KEYWORD_SIGN_BIT_RIGHT "вправо"
#define KAR_KEYWORD_SIGN_BIT_LEFT "влево"

// Команды алгоритма.
#define KAR_KEYWORD_COMMAND_RETURN "вернуть"

#define KAR_KEYWORD_COMMAND_BLOCK "блок"
#define KAR_KEYWORD_COMMAND_PASS "пропустить"

#define KAR_KEYWORD_COMMAND_IF "если"
#define KAR_KEYWORD_COMMAND_ELSE "иначе"

#define KAR_KEYWORD_COMMAND_CLEAN "раскрыть"
#define KAR_KEYWORD_COMMAND_AS "как"

#define KAR_KEYWORD_COMMAND_WHILE "пока"
#define KAR_KEYWORD_COMMAND_CONTINUE "следующий"
#define KAR_KEYWORD_COMMAND_BREAK "прервать"

#define KAR_KEYWORD_DECLARATION_METHOD "метод"

#define KAR_KEYWORD_STAT "стат"
#define KAR_KEYWORD_METHOD_MODIFIER_DYNAMIC "динамический"

#define KAR_KEYWORD_METHOD_MODIFIER_PRIVATE "закрытый"
#define KAR_KEYWORD_METHOD_MODIFIER_PROTECTED "защищённый"
#define KAR_KEYWORD_METHOD_MODIFIER_PUBLIC "открытый"

#define KAR_KEYWORD_METHOD_MODIFIER_FINALIZED "финализированный"
#define KAR_KEYWORD_METHOD_MODIFIER_INHERITED "наследуемый"

#define KAR_KEYWORD_METHOD_MODIFIER_OVERLOAD "перегруженный"

#endif // KAR_KEYWORD_H
