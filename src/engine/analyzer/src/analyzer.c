/* Copyright © 2020,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "analyzer/analyzer.h"

#include "model/token.h"

static bool fill_standard_lib(KarVars* vars) {
	KarVartree* root = kar_vartree_create_root();
    vars->vartree = root;
	
	KarVartree* libraries = kar_vartree_create_package("Библиотеки");
	kar_vartree_child_add(root, libraries);
	
	KarVartree* proj = kar_vartree_create_package("Проект");
	kar_vartree_child_add(root, proj);
	
	KarVartree* kar = kar_vartree_create_package("Кар");
	kar_vartree_child_add(libraries, kar);
	
	KarVartree* types = kar_vartree_create_package("Типы");
	kar_vartree_child_add(kar, types);
	
	KarVartree* type_bool = kar_vartree_create_bool("Буль");
	kar_vartree_child_add(types, type_bool);
	
	KarVartree* type_integer8 = kar_vartree_create_integer8("Целое8");
	kar_vartree_child_add(types, type_integer8);
	// TODO: Посмотреть, адекватно ли константы для всех типов записываются в памяти.
	kar_vartree_child_add(type_integer8, kar_vartree_create_const("Мин", type_integer8, (void*)0xFF));
	kar_vartree_child_add(type_integer8, kar_vartree_create_const("Макс", type_integer8, (void*)0x7F));
	
	KarVartree* type_integer16 = kar_vartree_create_integer16("Целое16");
	kar_vartree_child_add(types, type_integer16);
	kar_vartree_child_add(type_integer16, kar_vartree_create_const("Мин", type_integer16, (void*)0xFFFF));
	kar_vartree_child_add(type_integer16, kar_vartree_create_const("Макс", type_integer16, (void*)0x7FFF));
	
	KarVartree* type_integer32 = kar_vartree_create_integer32("Целое32");
	kar_vartree_child_add(types, type_integer32);
	kar_vartree_child_add(type_integer32, kar_vartree_create_const("Мин", type_integer32, (void*)0xFFFFFFFF));
	kar_vartree_child_add(type_integer32, kar_vartree_create_const("Макс", type_integer32, (void*)0x7FFFFFFF));
	
	KarVartree* type_integer64 = kar_vartree_create_integer64("Целое64");
	kar_vartree_child_add(types, type_integer64);
	kar_vartree_child_add(type_integer64, kar_vartree_create_const("Мин", type_integer64, (void*)0xFFFFFFFFFFFFFFFF));
	kar_vartree_child_add(type_integer64, kar_vartree_create_const("Макс", type_integer64, (void*)0x7FFFFFFFFFFFFFFF));
	
    KarVartree* type_integer = kar_vartree_create_class_link("Целое", type_integer32);
	kar_vartree_child_add(types, type_integer);
	
	KarVartree* type_unsigned8 = kar_vartree_create_unsigned8("Счётное8");
	kar_vartree_child_add(types, type_unsigned8);
	kar_vartree_child_add(type_unsigned8, kar_vartree_create_const("Макс", type_unsigned8, (void*)0xFF));
	
	KarVartree* type_unsigned16 = kar_vartree_create_unsigned16("Счётное16");
	kar_vartree_child_add(types, type_unsigned16);
	kar_vartree_child_add(type_unsigned16, kar_vartree_create_const("Макс", type_unsigned16, (void*)0xFFFF));
	
	KarVartree* type_unsigned32 = kar_vartree_create_unsigned32("Счётное32");
	kar_vartree_child_add(types, type_unsigned32);
	kar_vartree_child_add(type_unsigned32, kar_vartree_create_const("Макс", type_unsigned32, (void*)0xFFFFFFFF));
	
	KarVartree* type_unsigned64 = kar_vartree_create_unsigned64("Счётное64");
	kar_vartree_child_add(types, type_unsigned64);
	kar_vartree_child_add(type_unsigned64, kar_vartree_create_const("Макс", type_unsigned64, (void*)0xFFFFFFFFFFFFFFFF));
	
    KarVartree* type_unsigned = kar_vartree_create_class_link("Счётное", type_unsigned32);
	kar_vartree_child_add(types, type_unsigned);
	
	KarVartree* type_float32 = kar_vartree_create_float32("Дробное32");
	kar_vartree_child_add(types, type_float32);
	kar_vartree_child_add(type_float32, kar_vartree_create_const("БитыЭкспоненты", type_integer, (void*)7));
	kar_vartree_child_add(type_float32, kar_vartree_create_const("БитыМантиссы", type_integer, (void*)24));
	kar_vartree_child_add(type_float32, kar_vartree_create_const("ЭкспонентаМин", type_integer, (void*)-125));
	kar_vartree_child_add(type_float32, kar_vartree_create_const("ЭкспонентаМакс", type_integer, (void*)128));
	float f = 1.17549e-38f;
	kar_vartree_child_add(type_float32, kar_vartree_create_const("Мин", type_float32, (void*)((size_t)f)));
	f = 3.40282e+38f;
	kar_vartree_child_add(type_float32, kar_vartree_create_const("Макс", type_float32, (void*)((size_t)f)));
	f = 1.19209e-07f;
	kar_vartree_child_add(type_float32, kar_vartree_create_const("Эпсилон", type_float32, (void*)((size_t)f)));
	
	KarVartree* type_float64 = kar_vartree_create_float64("Дробное64");
	kar_vartree_child_add(types, type_float64);
	kar_vartree_child_add(type_float64, kar_vartree_create_const("БитыЭкспоненты", type_integer, (void*)10));
	kar_vartree_child_add(type_float64, kar_vartree_create_const("БитыМантиссы", type_integer, (void*)53));
	kar_vartree_child_add(type_float64, kar_vartree_create_const("ЭкспонентаМин", type_integer, (void*)-1021));
	kar_vartree_child_add(type_float64, kar_vartree_create_const("ЭкспонентаМакс", type_integer, (void*)1024));
	double d = 2.22507e-308;
	kar_vartree_child_add(type_float64, kar_vartree_create_const("Мин", type_float64, (void*)((size_t)d)));
	d = 1.79769e+308;
	kar_vartree_child_add(type_float64, kar_vartree_create_const("Макс", type_float64, (void*)((size_t)d)));
	d = 2.22045e-16;
	kar_vartree_child_add(type_float64, kar_vartree_create_const("Эпсилон", type_float64, (void*)((size_t)d)));
	
    KarVartree* type_float = kar_vartree_create_class_link("Дробное", type_float64);
	kar_vartree_child_add(types, type_float);
	
	KarVartree* type_unclean = kar_vartree_create_unclean("Неопределённость");
	kar_vartree_child_add(types, type_unclean);
	
	KarVartree* type_unclean_unsigned = kar_vartree_create_unclean_module(type_unsigned);
	kar_vartree_child_add(type_unclean, type_unclean_unsigned);
	
	KarVartree* type_string = kar_vartree_create_string("Строка");
	kar_vartree_child_add(types, type_string);
    kar_vartree_child_add(type_string, kar_vartree_create_function("Длина", "_kartarika_library_string_length", NULL, 0, type_unsigned));
    kar_vartree_child_add(type_string, kar_vartree_create_function("Найти", "_kartarika_library_string_find", &type_string, 1, type_unclean_unsigned));
	KarVartree* string_find_from_args[] = {type_unsigned, type_string};
    kar_vartree_child_add(type_string, kar_vartree_create_function("НайтиСПозиции", "_kartarika_library_string_find_from", string_find_from_args, 2, type_unclean_unsigned));
	KarVartree* string_substring_args[] = {type_unsigned, type_unsigned};
    kar_vartree_child_add(type_string, kar_vartree_create_function("Подстрока", "_kartarika_library_string_substring", string_substring_args, 2, type_string));
	
	KarVartree* type_unclean_string = kar_vartree_create_unclean_module(type_string);
	kar_vartree_child_add(type_unclean, type_unclean_string);

    KarVartree* type_console = kar_vartree_create_class("Консоль");
	kar_vartree_child_add(kar, type_console);
    kar_vartree_child_add(type_console, kar_vartree_create_function("Ввод", "_kartarika_library_read_string", NULL, 0, type_unclean_string));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_bool", &type_bool, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_int8", &type_integer8, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_int16", &type_integer16, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_int32", &type_integer32, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_int64", &type_integer64, 1, NULL));
    // TODO: Возможно это  можно убрать, так как ссылка.
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_int32", &type_integer, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint8", &type_unsigned8, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint16", &type_unsigned16, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint32", &type_unsigned32, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint64", &type_unsigned64, 1, NULL));
    // TODO: Возможно это  можно убрать, так как ссылка.
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint32", &type_unsigned, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_float32", &type_float32, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_float64", &type_float64, 1, NULL));
    kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_float64", &type_float, 1, NULL));
	// TODO: Поменять функцию на "_kartarika_library_write_string"
	kar_vartree_child_add(type_console, kar_vartree_create_function("Вывод", "_kartarika_library_write_chars", &type_string, 1, NULL));

    KarVartree* type_thread_error = kar_vartree_create_class("ПотокОшибок");
    kar_vartree_child_add(kar, type_thread_error);
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_bool", &type_bool, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_int8", &type_integer8, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_int16", &type_integer16, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_int32", &type_integer32, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_int64", &type_integer64, 1, NULL));
    // TODO: Возможно это  можно убрать, так как ссылка.
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_int32", &type_integer, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint8", &type_unsigned8, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint16", &type_unsigned16, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint32", &type_unsigned32, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint64", &type_unsigned64, 1, NULL));
    // TODO: Возможно это  можно убрать, так как ссылка.
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_uint32", &type_unsigned, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_float32", &type_float32, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_float64", &type_float64, 1, NULL));
    kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_float64", &type_float, 1, NULL));
	// TODO: Поменять функцию на "_kartarika_library_write_string"
	kar_vartree_child_add(type_thread_error, kar_vartree_create_function("Вывод", "_kartarika_library_write_chars", &type_string, 1, NULL));

    KarVartree* type_math = kar_vartree_create_class("Мат");
	kar_vartree_child_add(kar, type_math);
    kar_vartree_child_add(type_math, kar_vartree_create_function("ВзятьПСЧ", "_kartarika_library_get_prn", &type_unsigned, 1, NULL));
    kar_vartree_child_add(type_math, kar_vartree_create_function("ВзятьСлучайное", "_kartarika_library_get_random", NULL, 0, type_unsigned));
	

    kar_vars_default_list_add(vars, types);
    kar_vars_default_list_add(vars, kar);
    kar_vars_default_list_add(vars, root);

    vars->standard.boolType = type_bool;
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
