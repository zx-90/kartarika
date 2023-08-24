/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021,2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "tester/test.h"

#include <string.h>
#include <stdio.h>

#include "core/alloc.h"
#include "core/file_system.h"
#include "core/stream.h"
#include "core/string.h"
#include "model/project.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "analyzer/analyzer.h"
#include "generator/generator.h"

void kar_path_element_init(KarPathElement* element) {
	element->is = false;
	element->path = NULL;
}

void kar_path_element_clear(KarPathElement* element) {
	if (element->path) {
		KAR_FREE(element->path);
	}
	element->is = false;
}

bool kar_path_element_set(KarPathElement* element, const KarString* path) {
	if (element->path != NULL) {
		KAR_FREE(element->path);
	}
	
	element->path = kar_string_create(path);
	element->is = true;
	
	return true;
}

KarTest* kar_test_create() {
	KAR_CREATE(result, KarTest);
	
	kar_path_element_init(&result->project_file);
	kar_path_element_init(&result->lexer_error_file);
	kar_path_element_init(&result->lexer_file);
	kar_path_element_init(&result->parser_error_file);
	kar_path_element_init(&result->parser_file);
	kar_path_element_init(&result->compiler_error_file);
	kar_path_element_init(&result->out_file);
	kar_path_element_init(&result->comment_file);
	
	return result;
}

void kar_test_free(KarTest* test) {
	kar_path_element_clear(&test->project_file);
	kar_path_element_clear(&test->lexer_error_file);
	kar_path_element_clear(&test->lexer_file);
	kar_path_element_clear(&test->parser_error_file);
	kar_path_element_clear(&test->parser_file);
	kar_path_element_clear(&test->compiler_error_file);
	kar_path_element_clear(&test->out_file);
	kar_path_element_clear(&test->comment_file);
	
	KAR_FREE(test);
}

static KarError* check_file_object(const KarString* path, bool checkIsFile, KarPathElement* result) {
	if (checkIsFile) {
		if (kar_file_system_is_file(path)) {
			kar_path_element_set(result, path);
			return NULL;
		} else {
			return kar_error_register(1, "Объект %s не является файлом.", path);
		}
	} else {
		if (kar_file_system_is_directory(path)) {
			kar_path_element_set(result, path);
			return NULL;
		} else {
			return kar_error_register(1, "Объект %s не является каталогом.", path);
		}
	}
	return NULL;
}

static KarError* check_for_test_directory(KarTest* test, KarStringList* files) {
	KarError* error = NULL;
	size_t i;
	for (i = 0; i < kar_string_list_count(files); ++i) {
		KarString* file = kar_string_list_get(files, i);
		const KarString* filename = kar_file_system_get_basename(file);
		if (kar_string_equal(filename, KAR_PROJECT_FILENAME)) {
			error = check_file_object(file, true, &test->project_file);
		} else if (kar_string_equal(filename, KAR_LEXER_ERROR_FILENAME)) {
			error = check_file_object(file, true, &test->lexer_error_file);
		} else if (kar_string_equal(filename, KAR_LEXER_FILENAME)) {
			error = check_file_object(file, true, &test->lexer_file);
		} else if (kar_string_equal(filename, KAR_PARSER_ERROR_FILENAME)) {
			error = check_file_object(file, true, &test->parser_error_file);
		} else if (kar_string_equal(filename, KAR_PARSER_FILENAME)) {
			error = check_file_object(file, true, &test->parser_file);
		} else if (kar_string_equal(filename, KAR_COMPILER_ERROR_FILENAME)) {
			error = check_file_object(file, true, &test->compiler_error_file);
		} else if (kar_string_equal(filename, KAR_OUT_FILENAME)) {
			error = check_file_object(file, true, &test->out_file);
		} else if (kar_string_equal(filename, KAR_COMMENT_FILENAME)) {
			error = check_file_object(file, true, &test->comment_file);
		} else {
			return kar_error_register(1,
				"Объект %s лишний для тестового каталога.\nДопустимые имена: %s, %s, %s, %s, %s, %s, %s, %s.",
				filename,
				KAR_PROJECT_FILENAME,
				KAR_LEXER_ERROR_FILENAME, 
				KAR_LEXER_FILENAME,
				KAR_PARSER_ERROR_FILENAME,
				KAR_PARSER_FILENAME,
				KAR_COMPILER_ERROR_FILENAME,
				KAR_OUT_FILENAME,
				KAR_COMMENT_FILENAME
			);
		}
		if (error) {
			return error;
		}
	}
	return NULL;
}

static KarError* check_for_integrity(KarTest* test) {
	if (!test->project_file.is) {
		return kar_error_register(1, "В тесте отсутствует файл %s. Нет входных данных для теста.", KAR_PROJECT_FILENAME);
	}
	if (
		!test->lexer_error_file.is && !test->lexer_file.is &&
		!test->parser_error_file.is && !test->parser_file.is &&
		!test->compiler_error_file.is && !test->out_file.is
	) {
		return kar_error_register(1, "В тесте нет ни одного шаблона для сравнения. Нет выходных данных для сравнения результата тестов.");
	}
	
	if (test->lexer_error_file.is) {
		if (test->lexer_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Результат работы лексера не может быть одновременно положительным и отрицательным.", KAR_LEXER_ERROR_FILENAME, KAR_LEXER_FILENAME);
		}
		if (test->parser_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то парсер уже не запускается.", KAR_LEXER_ERROR_FILENAME, KAR_PARSER_ERROR_FILENAME);
		}
		if (test->parser_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то парсер уже не запускается.", KAR_LEXER_ERROR_FILENAME, KAR_PARSER_FILENAME);
		}
		if (test->compiler_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то компилятор уже не запускается.", KAR_LEXER_ERROR_FILENAME, KAR_COMPILER_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то программа не скомпилируется и не запустится.", KAR_LEXER_ERROR_FILENAME, KAR_OUT_FILENAME);
		}
	}
	
	if (test->parser_error_file.is) {
		if (test->parser_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Результат работы парсера не может быть одновременно положительным и отрицательным.", KAR_PARSER_ERROR_FILENAME, KAR_PARSER_FILENAME);
		}
		if (test->compiler_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то компилятор уже не запускается.", KAR_PARSER_ERROR_FILENAME, KAR_COMPILER_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то программа не скомпилируется и не запустится.", KAR_PARSER_ERROR_FILENAME, KAR_OUT_FILENAME);
		}
	}
	
	if (test->compiler_error_file.is) {
		if (test->out_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если компилятор выдает ошибку, то программа не скомпилируется и не запустится.", KAR_COMPILER_ERROR_FILENAME, KAR_OUT_FILENAME);
		}
	}
	
	return NULL;
}

static KarError* fill_test(KarTest* test, const KarString* dir) {
	KarStringList* files = kar_file_create_absolute_directory_list(dir);
	KarError* error;
	error = check_for_test_directory(test, files);
	kar_string_list_free(files);
	
	if (error) {
		return error;
	}
	
	return check_for_integrity(test);
}

// TODO: Возможно нужно перенести в Core.
static KarCursor* compare_strings(KarString* str1, KarString* str2) {
	KAR_CREATE(result, KarCursor);
	kar_cursor_init(result);

	while(*str1) {
		if (*str1 == '\r') {
			str1++;
			continue;
		}
		if (*str2 == '\r') {
			str2++;
			continue;
		}
		if (*str1 != *str2) {
			return result;
		}
		str1++;
		str2++;
		if (*str1 == '\n') {
			kar_cursor_next_line(result);
		} else {
			kar_cursor_next(result);
		}
	}
	
	if (*str2) {
		return result;
	}
	
	KAR_FREE(result);
	return NULL;
}

KarError* kar_test_run(KarTest* test, const KarString* dir) {
	KarError* error = fill_test(test, dir);
	if (error) {
		return error;
	}
	
	// TODO: "/" Получать через ОС, и вообще перенести в file_system или куда-то туда.
	KarString* path2 = kar_string_create_concat(dir, KAR_FILE_SYSTEM_DELIMETER);
	KarProject* project = kar_project_create(test->project_file.path);
	
	{
		KarStream* file = kar_stream_create(test->project_file.path);
        bool lexerResult = kar_lexer_run(file, project->module, project->errors);
		kar_stream_free(file);
		if (test->lexer_file.is) {
			if (!lexerResult) {
                kar_project_error_list_print(project->errors);
				kar_project_free(project);
				KAR_FREE(path2);
				return kar_error_register(1, "Ошибка в лексере. Ожидалось, что лексер отработает нормально.");
			}
			
			KarString* testResult = kar_token_create_print(project->module->token);
			KarString* gold_path = kar_string_create_concat(path2, kar_file_system_get_basename(test->lexer_file.path));
			KarString* gold = kar_file_load(gold_path);
			KAR_FREE(gold_path);
			
			KarCursor* cursor = compare_strings(testResult, gold);
			if (cursor) {
				KarError* result = kar_error_register(1, "Ошибка в лексере. Выход теста не совпадает с ожидаемым [%d;%d].\n"
					"Эталон:\n%s\nВывод программы:\n%s",
					cursor->line, cursor->column, gold, testResult
				);
				kar_project_free(project);
				KAR_FREE(path2);
				KAR_FREE(testResult);
				KAR_FREE(gold);
				KAR_FREE(cursor);
				return result;
			}
			KAR_FREE(testResult);
			KAR_FREE(gold);
			KAR_FREE(cursor);
		} else if (test->lexer_error_file.is) {
			if (lexerResult) {
				kar_project_free(project);
				KAR_FREE(path2);
				return kar_error_register(1, "Ошибка в лексере. Ожидалось, что лексер вернет ошибку, но он отработал нормально.");
			}
		} else if (!lexerResult) {
			kar_project_free(project);
			KAR_FREE(path2);
			return kar_error_register(1, "Ошибка в лексере. Ожидалось, что лексер отработает нормально и продолжится тестирование следующих модулей.");
		}
	}
	
	if (
		test->parser_error_file.is ||
		test->parser_file.is ||
		test->compiler_error_file.is ||
		test->out_file.is
	) {
        bool parserResult = kar_parser_run(project->module, project->errors);
		if (test->parser_file.is) {
			if (!parserResult) {
				KarString* moduleResult = kar_token_create_print(project->module->token);
				KarError* result = kar_error_register(1, "Ошибка в парсере. Ожидалось, что парсер отработает нормально.\n"
					"Структура модуля:\n%s",
					moduleResult
				);
				KAR_FREE(moduleResult);
                kar_project_error_list_print(project->errors);
				kar_project_free(project);
				KAR_FREE(path2);
				return result;
			}
			
			KarString* testResult = kar_token_create_print(project->module->token);
			KarString* gold_path = kar_string_create_concat(path2, kar_file_system_get_basename(test->parser_file.path));
			KarString* gold = kar_file_load(gold_path);
			KAR_FREE(gold_path);
			
			KarCursor* cursor = compare_strings(testResult, gold);
			if (cursor) {
				KarError* result = kar_error_register(1, "Ошибка в парсере. Выход теста не совпадает с ожидаемым [%d;%d].\n"
					"Эталон:\n%s\nВывод программы:\n%s",
					cursor->line, cursor->column, gold, testResult
				);
				kar_project_free(project);
				KAR_FREE(path2);
				KAR_FREE(testResult);
				KAR_FREE(gold);
				KAR_FREE(cursor);
				return result;
			}
			KAR_FREE(testResult);
			KAR_FREE(gold);
			KAR_FREE(cursor);
		} else if (test->parser_error_file.is) {
			if (parserResult) {
				KarString* moduleResult = kar_token_create_print(project->module->token);
				KarError* result = kar_error_register(1, "Ошибка в парсере. Ожидалось, что парсер вернет ошибку, но он отработал нормально.\n"
					"Структура модуля:\n%s",
					moduleResult
				);
				KAR_FREE(moduleResult);
				kar_project_free(project);
				KAR_FREE(path2);
				return result;
			}
		} else if (!parserResult) {
			kar_project_free(project);
			KAR_FREE(path2);
			return kar_error_register(1, "Ошибка в парсере. Ожидалось, что парсер отработает нормально и продолжится тестирование следующих модулей.");
		}
	}
	
	if (
		test->compiler_error_file.is ||
		test->out_file.is
	) {
		// TODO: Компилировать и собирать исполняемый файл в отдельном каталоге.
        bool generatorResult = kar_analyzer_run(project) && kar_generator_run(project);
		if (generatorResult) {
			if (test->compiler_error_file.is) {
				kar_project_free(project);
				KAR_FREE(path2);
				return kar_error_register(1, "Ошибка в компиляторе. Ожидалось, что компилятор вернет ошибку, но он отработал нормально.");
			}
		} else {
			if (!test->compiler_error_file.is) {
				kar_project_error_list_print(project->errors);
				KarError* result = kar_error_register(1, "Ошибка в компиляторе. Ожидалось, что компилятор отработает нормально, но он вернул ошибку.");
				kar_project_free(project);
				KAR_FREE(path2);
				return result;
			}
		}
	}
	
	if (test->out_file.is) {
		// TODO: Зависит от ОС. Перетащить в соответствующий модуль.
		#ifdef __linux__
		if (system("./a.out > out.txt 2>&1") < 0) {
			KarError* result = kar_error_register(1, "Ошибка при линковке программы");
			kar_project_free(project);
			KAR_FREE(path2);
			return result;
		}
		#elif _WIN32
				system("a.exe > out.txt 2>&1");
		#endif

		KarString* gold_path = kar_string_create_concat(path2, kar_file_system_get_basename(test->out_file.path));
		KarString* gold = kar_file_load(gold_path);
		KAR_FREE(gold_path);
		KarString* out = kar_file_load("out.txt");

		KarCursor* cursor = compare_strings(out, gold);

		if (cursor) {
			KarError* result = kar_error_register(1, "Ошибка в выходном потоке. Вывод программы не совпадает с ожидаемым [%d;%d].\n"
				"Эталон:\n%s\nВывод программы:\n%s\n",
				cursor->line, cursor->column, gold, out
			);
			kar_project_free(project);
			KAR_FREE(path2);
			KAR_FREE(out);
			KAR_FREE(gold);
			KAR_FREE(cursor);
			return result;
		} 
	} else {
		
	}

	// TODO: Эта строка для вывода всех сообщений об ошибках.
	// kar_module_print_errors(project->module);
	
	kar_project_free(project);
	KAR_FREE(path2);
	printf("OK\n");
	fflush(stdout);
	return NULL;
}
