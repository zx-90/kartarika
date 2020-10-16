/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
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
#include "core/module.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "generator/generator.h"

void kar_path_element_init(KarPathElement* element) {
	element->is = false;
	element->path = NULL;
}

bool kar_path_element_set(KarPathElement* element, const char* path) {
	if (element->path != NULL) {
		KAR_FREE(element->path);
	}
	
	size_t len = strlen(path);
	KAR_ALLOCS(element->path, char, len + 1);
	if (!element->path) {
		return false;
	}
	strcpy(element->path, path);
	
	element->is = true;
	
	return true;
}

KarTest* kar_test_create() {
	KAR_CREATE(result, KarTest);
	if (!result) {
		return NULL;
	}
	
	kar_path_element_init(&result->project_file);
	kar_path_element_init(&result->lexer_error_file);
	kar_path_element_init(&result->lexer_file);
	kar_path_element_init(&result->parser_error_file);
	kar_path_element_init(&result->parser_file);
	kar_path_element_init(&result->analyzer_error_file);
	kar_path_element_init(&result->analyzer_file);
	kar_path_element_init(&result->compiler_error_file);
	kar_path_element_init(&result->out_error_file);
	kar_path_element_init(&result->out_file);
	kar_path_element_init(&result->comment_file);
	
	return result;
}

void kar_test_free(KarTest* test) {
	KAR_FREE(test);
}

static KarError* check_file_object(const char* path, bool checkIsFile, KarPathElement* result) {
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

static KarError* check_for_test_directory(KarTest* test, char** files, size_t file_count) {
	KarError* error = NULL;
	size_t i;
	for (i = 0; i < file_count; ++i) {
		const char* filename = files[i];
		if (!strcmp(filename, KAR_PROJECT_FILENAME)) {
			error = check_file_object(filename, true, &test->project_file);
		} else if (!strcmp(filename, KAR_LEXER_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->lexer_error_file);
		} else if (!strcmp(filename, KAR_LEXER_FILENAME)) {
			error = check_file_object(filename, true, &test->lexer_file);
		} else if (!strcmp(filename, KAR_PARSER_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->parser_error_file);
		} else if (!strcmp(filename, KAR_PARSER_FILENAME)) {
			error = check_file_object(filename, true, &test->parser_file);
		} else if (!strcmp(filename, KAR_ANALYZER_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->analyzer_error_file);
		} else if (!strcmp(filename, KAR_ANALYZER_FILENAME)) {
			error = check_file_object(filename, true, &test->analyzer_file);
		} else if (!strcmp(filename, KAR_COMPILER_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->compiler_error_file);
		} else if (!strcmp(filename, KAR_OUT_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->out_error_file);
		} else if (!strcmp(filename, KAR_OUT_FILENAME)) {
			error = check_file_object(filename, true, &test->out_file);
		} else if (!strcmp(filename, KAR_COMMENT_FILENAME)) {
			error = check_file_object(filename, true, &test->comment_file);
		} else {
			return kar_error_register(1,
				"Объект %s лишний для тестового каталога.\nДопустимые имена: %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s.",
				filename,
				KAR_PROJECT_FILENAME,
				KAR_LEXER_ERROR_FILENAME, 
				KAR_LEXER_FILENAME,
				KAR_PARSER_ERROR_FILENAME,
				KAR_PARSER_FILENAME,
				KAR_ANALYZER_ERROR_FILENAME,
				KAR_ANALYZER_FILENAME,
				KAR_COMPILER_ERROR_FILENAME,
				KAR_OUT_ERROR_FILENAME,
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
		!test->analyzer_error_file.is && !test->analyzer_file.is &&
		!test->compiler_error_file.is &&
		!test->out_error_file.is && !test->out_file.is
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
		if (test->analyzer_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то анализатор уже не запускается.", KAR_LEXER_ERROR_FILENAME, KAR_ANALYZER_ERROR_FILENAME);
		}
		if (test->analyzer_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то анализатор уже не запускается.", KAR_LEXER_ERROR_FILENAME, KAR_ANALYZER_FILENAME);
		}
		if (test->compiler_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то компилятор уже не запускается.", KAR_LEXER_ERROR_FILENAME, KAR_COMPILER_ERROR_FILENAME);
		}
		if (test->out_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то программа не скомпилируется и не запустится.", KAR_LEXER_ERROR_FILENAME, KAR_OUT_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то программа не скомпилируется и не запустится.", KAR_LEXER_ERROR_FILENAME, KAR_OUT_FILENAME);
		}
	}
	
	if (test->parser_error_file.is) {
		if (test->parser_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Результат работы парсера не может быть одновременно положительным и отрицательным.", KAR_PARSER_ERROR_FILENAME, KAR_PARSER_FILENAME);
		}
		if (test->analyzer_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то анализатор уже не запускается.", KAR_PARSER_ERROR_FILENAME, KAR_ANALYZER_ERROR_FILENAME);
		}
		if (test->analyzer_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то анализатор уже не запускается.", KAR_PARSER_ERROR_FILENAME, KAR_ANALYZER_FILENAME);
		}
		if (test->compiler_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то компилятор уже не запускается.", KAR_PARSER_ERROR_FILENAME, KAR_COMPILER_ERROR_FILENAME);
		}
		if (test->out_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то программа не скомпилируется и не запустится.", KAR_PARSER_ERROR_FILENAME, KAR_OUT_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то программа не скомпилируется и не запустится.", KAR_PARSER_ERROR_FILENAME, KAR_OUT_FILENAME);
		}
	}
	
	if (test->analyzer_error_file.is) {
		if (test->analyzer_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Результат работы анализатора не может быть одновременно положительным и отрицательным.", KAR_ANALYZER_ERROR_FILENAME, KAR_ANALYZER_FILENAME);
		}
		if (test->compiler_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если анализатор выдает ошибку, то компилятор уже не запускается.", KAR_ANALYZER_ERROR_FILENAME, KAR_COMPILER_ERROR_FILENAME);
		}
		if (test->out_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если анализатор выдает ошибку, то программа не скомпилируется и не запустится.", KAR_ANALYZER_ERROR_FILENAME, KAR_OUT_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если анализатор выдает ошибку, то программа не скомпилируется и не запустится.", KAR_ANALYZER_ERROR_FILENAME, KAR_OUT_FILENAME);
		}
	}
	
	if (test->compiler_error_file.is) {
		if (test->out_error_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если компилятор выдает ошибку, то программа не скомпилируется и не запустится.", KAR_COMPILER_ERROR_FILENAME, KAR_OUT_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return kar_error_register(1, "В тесте присутствуют одновременно %s и %s. Если компилятор выдает ошибку, то программа не скомпилируется и не запустится.", KAR_COMPILER_ERROR_FILENAME, KAR_OUT_FILENAME);
		}
	}
	
	return NULL;
}

KarError* kar_test_run(KarTest* test, const char* dir) {
	size_t file_count = 0;
	char** files = kar_file_create_directory_list(dir, &file_count);
	KarError* error;
	error = check_for_test_directory(test, files, file_count);
	if (error) {
		return error;
	}
	
	error = check_for_integrity(test);
	if (error) {
		return error;
	}
	
	// TODO: "/" Получать через ОС, и вообще перенести в file_system или куда-то туда.
	char* path2 = kar_string_create_concat(dir, "/");
	char* project_path = kar_string_create_concat(path2, test->project_file.path);
	KarStream* file = kar_stream_create(project_path);
	KAR_FREE(project_path);
	KarModule* module = kar_module_create(test->project_file.path);
	
	{
		bool lexerResult = kar_lexer_run(file, module);
		if (test->lexer_file.is) {
			if (!lexerResult) {
				kar_module_free(module);
				return kar_error_register(1, "Ошибка в лексере. Ожидалось, что лексер отработает нормально.");
			}
			
			char* testResult = kar_token_create_print(module->token);
			char* gold_path = kar_string_create_concat(path2, test->lexer_file.path);
			char* gold = kar_file_load(gold_path);
			KAR_FREE(gold_path);
			
			// TODO: Сделать сравнение более подробно хотя бы номер первой строки, в которой не сопадает исходный файл с тестом.
			if (strcmp(testResult, gold)) {
				kar_module_free(module);
				return kar_error_register(1, "Ошибка в лексере. Выход теста не совпадает с ожидаемым.\n"
					"Эталон:\n%s\nВывод программы:\n%s",
					gold, testResult
				);
			}
		} else if (test->lexer_error_file.is) {
			if (!lexerResult) {
				// TODO: Возможно в файле сообщения об ошибке можно добавить формат для сравнения результатов.
				return NULL;
			} else {
				return kar_error_register(1, "Ошибка в лексере. Ожидалось, что лексер вернет ошибку, но он отработал нормально.");
			}
		}
	}
	
	if (
		test->parser_error_file.is ||
		test->parser_file.is ||
		test->analyzer_error_file.is ||
		test->analyzer_file.is ||
		test->compiler_error_file.is ||
		test->out_error_file.is ||
		test->out_file.is
	) {
		bool parserResult = kar_parser_run(module);
		if (test->parser_file.is) {
			if (!parserResult) {
				return kar_error_register(1, "Ошибка в парсере. Ожидалось, что парсер отработает нормально.");
			}
		
			char* testResult = kar_token_create_print(module->token);
			char* gold_path = kar_string_create_concat(path2, test->parser_file.path);
			char* gold = kar_file_load(gold_path);
			KAR_FREE(gold_path);
			
			// TODO: Сделать сравнение более подробно хотя бы номер первой строки, в которой не сопадает исходный файл с тестом.
			if (testResult != gold) {
				return kar_error_register(1, "Ошибка в парсере. Выход теста не совпадает с ожидаемым.");
			}
			
		} else if (test->parser_error_file.is) {
			if (!parserResult) {
				// TODO: Возможно в файле сообщения об ошибке можно добавить формат для сравнения результатов.
				return NULL;
			} else {
				return kar_error_register(1, "Ошибка в парсере. Ожидалось, что парсер вернет ошибку, но он отработал нормально.");
			}
		}
	}
	
	if (
		test->analyzer_error_file.is ||
		test->analyzer_file.is ||
		test->compiler_error_file.is ||
		test->out_error_file.is ||
		test->out_file.is
	) {
		// TODO: Здесь должно быть тестирование анализатора.
		
	}

	if (
		test->compiler_error_file.is ||
		test->out_error_file.is ||
		test->out_file.is
	) {
		bool generatorResult = kar_generator_run(module);
		if (test->compiler_error_file.is) {
			if (!generatorResult) {
				// TODO: Возможно в файле сообщения об ошибке можно добавить формат для сравнения результатов.
				return NULL;
			} else {
				return kar_error_register(1, "Ошибка в парсере. Ожидалось, что парсер вернет ошибку, но он отработал нормально.");
			}
		}
	}
	
	if (
		test->out_error_file.is ||
		test->out_file.is
	) {
		// TODO: Зависит от ОС. Перетащить в соответствующий модуль.
		system("./a.out > out.txt 2> error.txt");
		// TODO: доделать.
		if (test->out_file.is) {
		}
	}

	return NULL;
}
