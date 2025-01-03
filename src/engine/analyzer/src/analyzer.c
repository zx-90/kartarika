/* Copyright © 2020,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "analyzer/analyzer.h"

#include "model/token.h"
#include "model/vartree_function.h"
#include "model/vartree_var.h"
#include "model/vartree_class.h"
#include "model/vartree_template.h"
#include "model/vartree_package.h"

static bool fill_standard_lib(KarVars* vars) {
	// Основные модули.
	KarVartree* root = kar_vartree_create_root();
    vars->vartree = root;
	
	KarVartree* libraries = kar_vartree_create_package(root, "Библиотеки");
	
	KarVartree* proj = kar_vartree_create_package(root, "Проект");
	
	KarVartree* kar = kar_vartree_create_package(libraries, "Кар");
	
	KarVartree* types = kar_vartree_create_package(kar, "Типы");
	
	// Тип пусто.
	KarVartree* type_null = kar_vartree_create_class(types, "Пусто");

	// Основные типы.
	KarVartree* type_bool =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_BOOL,"Буль");
	
	KarVartree* type_decimal =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_0INTEGER, "0Десятичное");
	KarVartree* type_hexadecimal =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_0HEX, "0Шестнадцатеричное");
	KarVartree* type_literal_float =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_0FLOAT, "0Дробное");

	KarVartree* type_integer8 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_INTEGER8, "Целое8");
	// TODO: Посмотреть, адекватно ли константы для всех типов записываются в памяти.
	kar_vartree_create_const(type_integer8,"Мин", VAR_STATIC | VAR_PUBLIC, type_integer8, (void*)0xFF);
	kar_vartree_create_const(type_integer8,"Макс", VAR_STATIC | VAR_PUBLIC, type_integer8, (void*)0x7F);
	
	KarVartree* type_integer16 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_INTEGER16, "Целое16");
	kar_vartree_create_const(type_integer16,"Мин", VAR_STATIC | VAR_PUBLIC, type_integer16, (void*)0xFFFF);
	kar_vartree_create_const(type_integer16,"Макс", VAR_STATIC | VAR_PUBLIC, type_integer16, (void*)0x7FFF);
	
	KarVartree* type_integer32 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_INTEGER32, "Целое32");
	kar_vartree_create_const(type_integer32, "Мин", VAR_STATIC | VAR_PUBLIC, type_integer32, (void*)0xFFFFFFFF);
	kar_vartree_create_const(type_integer32, "Макс", VAR_STATIC | VAR_PUBLIC, type_integer32, (void*)0x7FFFFFFF);
	
	KarVartree* type_integer64 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_INTEGER64, "Целое64");
	kar_vartree_create_const(type_integer64, "Мин", VAR_STATIC | VAR_PUBLIC, type_integer64, (void*)0xFFFFFFFFFFFFFFFF);
	kar_vartree_create_const(type_integer64, "Макс", VAR_STATIC | VAR_PUBLIC, type_integer64, (void*)0x7FFFFFFFFFFFFFFF);
	
	KarVartree* type_integer =
		kar_vartree_create_class_link(types, "Целое", type_integer32);
	
	KarVartree* type_unsigned8 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_UNSIGNED8, "Счётное8");
	kar_vartree_create_const(type_unsigned8, "Макс", VAR_STATIC | VAR_PUBLIC, type_unsigned8, (void*)0xFF);
	
	KarVartree* type_unsigned16 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_UNSIGNED16, "Счётное16");
	kar_vartree_create_const(type_unsigned16, "Макс", VAR_STATIC | VAR_PUBLIC, type_unsigned16, (void*)0xFFFF);
	
	KarVartree* type_unsigned32 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_UNSIGNED32, "Счётное32");
	kar_vartree_create_const(type_unsigned32, "Макс", VAR_STATIC | VAR_PUBLIC, type_unsigned32, (void*)0xFFFFFFFF);
	
	KarVartree* type_unsigned64 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_UNSIGNED64, "Счётное64");
	kar_vartree_create_const(type_unsigned64, "Макс", VAR_STATIC | VAR_PUBLIC, type_unsigned64, (void*)0xFFFFFFFFFFFFFFFF);
	
	KarVartree* type_unsigned =
		kar_vartree_create_class_link(types, "Счётное", type_unsigned32);
	
	KarVartree* type_float32 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_FLOAT32, "Дробное32");
	kar_vartree_create_const(type_float32, "БитыЭкспоненты", VAR_STATIC | VAR_PUBLIC, type_integer, (void*)7);
	kar_vartree_create_const(type_float32, "БитыМантиссы", VAR_STATIC | VAR_PUBLIC, type_integer, (void*)24);
	kar_vartree_create_const(type_float32, "ЭкспонентаМин", VAR_STATIC | VAR_PUBLIC, type_integer, (void*)-125);
	kar_vartree_create_const(type_float32, "ЭкспонентаМакс", VAR_STATIC | VAR_PUBLIC, type_integer, (void*)128);
	float f = 1.17549e-38f;
	kar_vartree_create_const(type_float32, "Мин", VAR_STATIC | VAR_PUBLIC, type_float32, (void*)((size_t)f));
	f = 3.40282e+38f;
	kar_vartree_create_const(type_float32, "Макс", VAR_STATIC | VAR_PUBLIC, type_float32, (void*)((size_t)f));
	f = 1.19209e-07f;
	kar_vartree_create_const(type_float32, "Эпсилон", VAR_STATIC | VAR_PUBLIC, type_float32, (void*)((size_t)f));
	
	KarVartree* type_float64 =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_FLOAT64, "Дробное64");
	kar_vartree_create_const(type_float64, "БитыЭкспоненты", VAR_STATIC | VAR_PUBLIC, type_integer, (void*)10);
	kar_vartree_create_const(type_float64, "БитыМантиссы", VAR_STATIC | VAR_PUBLIC, type_integer, (void*)53);
	kar_vartree_create_const(type_float64, "ЭкспонентаМин", VAR_STATIC | VAR_PUBLIC, type_integer, (void*)-1021);
	kar_vartree_create_const(type_float64, "ЭкспонентаМакс", VAR_STATIC | VAR_PUBLIC, type_integer, (void*)1024);
	double d = 2.22507e-308;
	kar_vartree_create_const(type_float64, "Мин", VAR_STATIC | VAR_PUBLIC, type_float64, (void*)((size_t)d));
	d = 1.79769e+308;
	kar_vartree_create_const(type_float64, "Макс", VAR_STATIC | VAR_PUBLIC, type_float64, (void*)((size_t)d));
	d = 2.22045e-16;
	kar_vartree_create_const(type_float64, "Эпсилон", VAR_STATIC | VAR_PUBLIC, type_float64, (void*)((size_t)d));
	
	KarVartree* type_float =
		kar_vartree_create_class_link(types, "Дробное", type_float64);
	
	// Неопределённости.
	// TODO: Имена функций и переменных в библиотеке должны начинаться на "_kartarika_library".
	//       Необходимо проверить здесь и далее.
	KarVartree* type_unclean = kar_vartree_create_unclean(types, "Неопределённость");
	kar_vartree_create_function(type_unclean, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_bool = kar_vartree_create_unclean_class(types, type_bool);
	kar_vartree_create_function(type_unclean_bool, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_decimal = kar_vartree_create_unclean_class(types, type_decimal);
	kar_vartree_create_function(type_unclean_decimal, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_hexadecimal = kar_vartree_create_unclean_class(types, type_hexadecimal);
	kar_vartree_create_function(type_unclean_hexadecimal, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_literal_float = kar_vartree_create_unclean_class(types, type_literal_float);
	kar_vartree_create_function(type_unclean_literal_float, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_integer8 = kar_vartree_create_unclean_class(types, type_integer8);
	kar_vartree_create_function(type_unclean_integer8, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_integer16 = kar_vartree_create_unclean_class(types, type_integer16);
	kar_vartree_create_function(type_unclean_integer16, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_integer32 = kar_vartree_create_unclean_class(types, type_integer32);
	kar_vartree_create_function(type_unclean_integer32, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_integer64 = kar_vartree_create_unclean_class(types, type_integer64);
	kar_vartree_create_function(type_unclean_integer64, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	// TODO: Вообще нужно ли. Шаблон на ссылку класса, когда есть шаблон класса.
	KarVartree* type_unclean_integer = kar_vartree_create_unclean_class(types, type_integer32);

	KarVartree* type_unclean_unsigned8 = kar_vartree_create_unclean_class(types, type_unsigned8);
	kar_vartree_create_function(type_unclean_unsigned8, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_unsigned16 = kar_vartree_create_unclean_class(types, type_unsigned16);
	kar_vartree_create_function(type_unclean_unsigned16, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_unsigned32 = kar_vartree_create_unclean_class(types, type_unsigned32);
	kar_vartree_create_function(type_unclean_unsigned32, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_unsigned64 = kar_vartree_create_unclean_class(types, type_unsigned64);
	kar_vartree_create_function(type_unclean_unsigned64, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	// TODO: Вообще нужно ли. Шаблон на ссылку класса, когда есть шаблон класса.
	KarVartree* type_unclean_unsigned = kar_vartree_create_unclean_class(types, type_unsigned32);

	KarVartree* type_unclean_float32 = kar_vartree_create_unclean_class(types, type_float32);
	kar_vartree_create_function(type_unclean_float32, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	KarVartree* type_unclean_float64 = kar_vartree_create_unclean_class(types, type_float64);
	kar_vartree_create_function(type_unclean_float64, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	// TODO: Вообще нужно ли. Шаблон на ссылку класса, когда есть шаблон класса.
	KarVartree* type_unclean_float = kar_vartree_create_unclean_class(types, type_float64);

	// Строки.
	KarVartree* type_string =
		kar_vartree_create_predefined_class(types, KAR_CLASS_TYPE_STRING, "Строка");
	kar_vartree_create_function(type_string, "Длина", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_library_string_length", NULL, 0, type_unsigned);
	kar_vartree_create_function(type_string, "Найти", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_library_string_find", &type_string, 1, type_unclean_unsigned);
	KarVartree* string_find_from_args[] = {type_unsigned, type_string};
	kar_vartree_create_function(type_string, "НайтиСПозиции", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_library_string_find_from", string_find_from_args, 2, type_unclean_unsigned);
	KarVartree* string_substring_args[] = {type_unsigned, type_unsigned};
	kar_vartree_create_function(type_string, "Подстрока", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_library_string_substring", string_substring_args, 2, type_string);
	
	KarVartree* type_unclean_string = kar_vartree_create_unclean_class(types, type_string);
	kar_vartree_create_function(type_unclean_string, "ПустойЛи", FUNC_DYNAMIC | FUNC_PUBLIC, "_kartarika_unclean_is_empty", NULL, 0, type_bool);

	// Преобразования типов.
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_bool", &type_bool, 1, type_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_bool", &type_integer8, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_bool", &type_integer16, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_bool", &type_integer32, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_bool", &type_integer64, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_bool", &type_unsigned8, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_bool", &type_unsigned16, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_bool", &type_unsigned32, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_bool", &type_unsigned64, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_bool", &type_float32, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_bool", &type_float64, 1, type_unclean_bool);
	kar_vartree_create_function(types, "Буль", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_bool", &type_string, 1, type_unclean_bool);

	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_integer8", &type_bool, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_integer8", &type_integer8, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_integer8", &type_integer16, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_integer8", &type_integer32, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_integer8", &type_integer64, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_integer8", &type_unsigned8, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_integer8", &type_unsigned16, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_integer8", &type_unsigned32, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_integer8", &type_unsigned64, 1, type_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_integer8", &type_float32, 1, type_unclean_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_integer8", &type_float64, 1, type_unclean_integer8);
	kar_vartree_create_function(types, "Целое8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_integer8", &type_string, 1, type_unclean_integer8);

	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_integer16", &type_bool, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_integer16", &type_integer8, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_integer16", &type_integer16, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_integer16", &type_integer32, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_integer16", &type_integer64, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_integer16", &type_unsigned8, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_integer16", &type_unsigned16, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_integer16", &type_unsigned32, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_integer16", &type_unsigned64, 1, type_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_integer16", &type_float32, 1, type_unclean_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_integer16", &type_float64, 1, type_unclean_integer16);
	kar_vartree_create_function(types, "Целое16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_integer16", &type_string, 1, type_unclean_integer16);

	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_integer32", &type_bool, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_integer32", &type_integer8, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_integer32", &type_integer16, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_integer32", &type_integer32, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_integer32", &type_integer64, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_integer32", &type_unsigned8, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_integer32", &type_unsigned16, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_integer32", &type_unsigned32, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_integer32", &type_unsigned64, 1, type_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_integer32", &type_float32, 1, type_unclean_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_integer32", &type_float64, 1, type_unclean_integer32);
	kar_vartree_create_function(types, "Целое32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_integer32", &type_string, 1, type_unclean_integer32);

	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_integer64", &type_bool, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_integer64", &type_integer8, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_integer64", &type_integer16, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_integer64", &type_integer32, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_integer64", &type_integer64, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_integer64", &type_unsigned8, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_integer64", &type_unsigned16, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_integer64", &type_unsigned32, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_integer64", &type_unsigned64, 1, type_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_integer64", &type_float32, 1, type_unclean_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_integer64", &type_float64, 1, type_unclean_integer64);
	kar_vartree_create_function(types, "Целое64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_integer64", &type_string, 1, type_unclean_integer64);

	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_unsigned8", &type_bool, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_unsigned8", &type_integer8, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_unsigned8", &type_integer16, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_unsigned8", &type_integer32, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_unsigned8", &type_integer64, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_unsigned8", &type_unsigned8, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_unsigned8", &type_unsigned16, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_unsigned8", &type_unsigned32, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_unsigned8", &type_unsigned64, 1, type_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_unsigned8", &type_float32, 1, type_unclean_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_unsigned8", &type_float64, 1, type_unclean_unsigned8);
	kar_vartree_create_function(types, "Счётное8", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_unsigned8", &type_string, 1, type_unclean_unsigned8);

	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_unsigned16", &type_bool, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_unsigned16", &type_integer8, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_unsigned16", &type_integer16, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_unsigned16", &type_integer32, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_unsigned16", &type_integer64, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_unsigned16", &type_unsigned8, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_unsigned16", &type_unsigned16, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_unsigned16", &type_unsigned32, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_unsigned16", &type_unsigned64, 1, type_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_unsigned16", &type_float32, 1, type_unclean_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_unsigned16", &type_float64, 1, type_unclean_unsigned16);
	kar_vartree_create_function(types, "Счётное16", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_unsigned16", &type_string, 1, type_unclean_unsigned16);

	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_unsigned32", &type_bool, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_unsigned32", &type_integer8, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_unsigned32", &type_integer16, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_unsigned32", &type_integer32, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_unsigned32", &type_integer64, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_unsigned32", &type_unsigned8, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_unsigned32", &type_unsigned16, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_unsigned32", &type_unsigned32, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_unsigned32", &type_unsigned64, 1, type_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_unsigned32", &type_float32, 1, type_unclean_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_unsigned32", &type_float64, 1, type_unclean_unsigned32);
	kar_vartree_create_function(types, "Счётное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_unsigned32", &type_string, 1, type_unclean_unsigned32);

	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_unsigned64", &type_bool, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_unsigned64", &type_integer8, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_unsigned64", &type_integer16, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_unsigned64", &type_integer32, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_unsigned64", &type_integer64, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_unsigned64", &type_unsigned8, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_unsigned64", &type_unsigned16, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_unsigned64", &type_unsigned32, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_unsigned64", &type_unsigned64, 1, type_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_unsigned64", &type_float32, 1, type_unclean_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_unsigned64", &type_float64, 1, type_unclean_unsigned64);
	kar_vartree_create_function(types, "Счётное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_unsigned64", &type_string, 1, type_unclean_unsigned64);

	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_float32", &type_bool, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_float32", &type_integer8, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_float32", &type_integer16, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_float32", &type_integer32, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_float32", &type_integer64, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_float32", &type_unsigned8, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_float32", &type_unsigned16, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_float32", &type_unsigned32, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_float32", &type_unsigned64, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_float32", &type_float32, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_float32", &type_float64, 1, type_float32);
	kar_vartree_create_function(types, "Дробное32", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_float32", &type_string, 1, type_unclean_float32);

	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_float64", &type_bool, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_float64", &type_integer8, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_float64", &type_integer16, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_float64", &type_integer32, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_float64", &type_integer64, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_float64", &type_unsigned8, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_float64", &type_unsigned16, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_float64", &type_unsigned32, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_float64", &type_unsigned64, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_float64", &type_float32, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_float64", &type_float64, 1, type_float64);
	kar_vartree_create_function(types, "Дробное64", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_float64", &type_string, 1, type_unclean_float64);

	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_bool_to_string", &type_bool, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer8_to_string", &type_integer8, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer16_to_string", &type_integer16, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer32_to_string", &type_integer32, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_integer64_to_string", &type_integer64, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned8_to_string", &type_unsigned8, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned16_to_string", &type_unsigned16, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned32_to_string", &type_unsigned32, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_unsigned64_to_string", &type_unsigned64, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float32_to_string", &type_float32, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_float64_to_string", &type_float64, 1, type_string);
	kar_vartree_create_function(types, "Строка", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_convert_string_to_string", &type_string, 1, type_string);

	// Стандартная библиотека.
	KarVartree* type_console = kar_vartree_create_class(kar, "Консоль");
	kar_vartree_create_function(type_console, "Ввод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_read_string", NULL, 0, type_unclean_string);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_bool", &type_bool, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int8", &type_integer8, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int16", &type_integer16, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int32", &type_integer32, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int64", &type_integer64, 1, NULL);
    // TODO: Возможно это  можно убрать, так как ссылка.
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int32", &type_integer, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint8", &type_unsigned8, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint16", &type_unsigned16, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint32", &type_unsigned32, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint64", &type_unsigned64, 1, NULL);
    // TODO: Возможно это  можно убрать, так как ссылка.
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint32", &type_unsigned, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_float32", &type_float32, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_float64", &type_float64, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_float64", &type_float, 1, NULL);
	kar_vartree_create_function(type_console, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_string", &type_string, 1, NULL);

	KarVartree* type_thread_error = kar_vartree_create_class(kar, "ПотокОшибок");
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_bool", &type_bool, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int8", &type_integer8, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int16", &type_integer16, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int32", &type_integer32, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int64", &type_integer64, 1, NULL);
    // TODO: Возможно это  можно убрать, так как ссылка.
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_int32", &type_integer, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint8", &type_unsigned8, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint16", &type_unsigned16, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint32", &type_unsigned32, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint64", &type_unsigned64, 1, NULL);
    // TODO: Возможно это  можно убрать, так как ссылка.
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_uint32", &type_unsigned, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_float32", &type_float32, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_float64", &type_float64, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_float64", &type_float, 1, NULL);
	kar_vartree_create_function(type_thread_error, "Вывод", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_write_string", &type_string, 1, NULL);

	KarVartree* type_math = kar_vartree_create_class(kar, "Мат");
	kar_vartree_create_function(type_math, "ВзятьПСЧ", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_get_prn", &type_unsigned, 1, NULL);
	kar_vartree_create_function(type_math, "ВзятьСлучайное", FUNC_STATIC | FUNC_PUBLIC, "_kartarika_library_get_random", NULL, 0, type_unsigned);
	
	// Список путей поиска переменных по-умолчанию.
    kar_vars_default_list_add(vars, types);
    kar_vars_default_list_add(vars, kar);
    kar_vars_default_list_add(vars, root);

	// Ссылки на модули.
	vars->standard.projectModule = proj;
	// Ссылки на стандартные типы.
	vars->standard.nullType = type_null;

	vars->standard.boolType = type_bool;
	vars->standard.decimalType = type_decimal;
	vars->standard.hexadecimalType = type_hexadecimal;
	vars->standard.literalFloatType = type_literal_float;
	vars->standard.int8Type = type_integer8;
    vars->standard.int16Type = type_integer16;
    vars->standard.int32Type = type_integer32;
    vars->standard.int64Type = type_integer64;
    vars->standard.intType = type_integer;
    vars->standard.unsigned8Type = type_unsigned8;
    vars->standard.unsigned16Type = type_unsigned16;
    vars->standard.unsigned32Type = type_unsigned32;
    vars->standard.unsigned64Type = type_unsigned64;
    vars->standard.unsignedType = type_unsigned;
    vars->standard.float32Type = type_float32;
    vars->standard.float64Type = type_float64;
    vars->standard.floatType = type_float;
    vars->standard.stringType = type_string;
	vars->standard.unclean = type_unclean;

	vars->standard.uncleanBool = type_unclean_bool;
	vars->standard.uncleanDecimal = type_unclean_decimal;
	vars->standard.uncleanHexadecimal = type_unclean_hexadecimal;
	vars->standard.uncleanLiteralFloat = type_unclean_literal_float;
	vars->standard.uncleanInt8 = type_unclean_integer8;
	vars->standard.uncleanInt16 = type_unclean_integer16;
	vars->standard.uncleanInt32 = type_unclean_integer32;
	vars->standard.uncleanInt64 = type_unclean_integer64;
	vars->standard.uncleanInt = type_unclean_integer;
	vars->standard.uncleanUnsigned8 = type_unclean_unsigned8;
	vars->standard.uncleanUnsigned16 = type_unclean_unsigned16;
	vars->standard.uncleanUnsigned32 = type_unclean_unsigned32;
	vars->standard.uncleanUnsigned64 = type_unclean_unsigned64;
	vars->standard.uncleanUnsigned = type_unclean_unsigned;
	vars->standard.uncleanFloat32 = type_unclean_float32;
	vars->standard.uncleanFloat64 = type_unclean_float64;
	vars->standard.uncleanFloat = type_unclean_float;
	vars->standard.uncleanString = type_unclean_string;

    return true;
}

bool kar_analyzer_run(KarProject* project) {
	if (!project) {
		return false;
	}
    if (!fill_standard_lib(project->vars)) {
		return false;
	}
	return true;
}
