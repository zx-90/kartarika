/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "tester/test.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "core/file_system.h"
#include "core/stream.h"
#include "core/string.h"
#include "core/module.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "generator/generator.h"

void k_path_element_init(KPathElement* element) {
	element->is = false;
	element->path = NULL;
}

bool k_path_element_set(KPathElement* element, const char* path) {
	if (element->path != NULL) {
		free(element->path);
	}
	
	size_t len = strlen(path);
	element->path = (char*)malloc(sizeof(char) * (len + 1));
	if (!element->path) {
		return false;
	}
	strcpy(element->path, path);
	
	element->is = true;
	
	return true;
}

KTest* k_test_create() {
	KTest* result = (KTest*)malloc(sizeof(KTest));
	if (!result) {
		return NULL;
	}
	
	k_path_element_init(&result->project_file);
	k_path_element_init(&result->lexer_error_file);
	k_path_element_init(&result->lexer_file);
	k_path_element_init(&result->parser_error_file);
	k_path_element_init(&result->parser_file);
	k_path_element_init(&result->analyzer_error_file);
	k_path_element_init(&result->analyzer_file);
	k_path_element_init(&result->compiler_error_file);
	k_path_element_init(&result->out_error_file);
	k_path_element_init(&result->out_file);
	k_path_element_init(&result->comment_file);
	
	return result;
}

void k_test_free(KTest* test) {
	free(test);
}

static KError* check_file_object(const char* path, bool checkIsFile, KPathElement* result) {
	if (checkIsFile) {
		if (k_file_system_is_file(path)) {
			k_path_element_set(result, path);
			return NULL;
		} else {
			return k_error_register(1, "Объект %s не является файлом.", path);
		}
	} else {
		if (k_file_system_is_directory(path)) {
			k_path_element_set(result, path);
			return NULL;
		} else {
			return k_error_register(1, "Объект %s не является каталогом.", path);
		}
	}
	return NULL;
}

static KError* check_for_test_directory(KTest* test, char** files, size_t file_count) {
	KError* error = NULL;
	size_t i;
	for (i = 0; i < file_count; ++i) {
		const char* filename = files[i];
		if (!strcmp(filename, PROJECT_FILENAME)) {
			error = check_file_object(filename, true, &test->project_file);
		} else if (!strcmp(filename, LEXER_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->lexer_error_file);
		} else if (!strcmp(filename, LEXER_FILENAME)) {
			error = check_file_object(filename, true, &test->lexer_file);
		} else if (!strcmp(filename, PARSER_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->parser_error_file);
		} else if (!strcmp(filename, PARSER_FILENAME)) {
			error = check_file_object(filename, true, &test->parser_file);
		} else if (!strcmp(filename, ANALYZER_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->analyzer_error_file);
		} else if (!strcmp(filename, ANALYZER_FILENAME)) {
			error = check_file_object(filename, true, &test->analyzer_file);
		} else if (!strcmp(filename, COMPILER_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->compiler_error_file);
		} else if (!strcmp(filename, OUT_ERROR_FILENAME)) {
			error = check_file_object(filename, true, &test->out_error_file);
		} else if (!strcmp(filename, OUT_FILENAME)) {
			error = check_file_object(filename, true, &test->out_file);
		} else if (!strcmp(filename, COMMENT_FILENAME)) {
			error = check_file_object(filename, true, &test->comment_file);
		} else {
			return k_error_register(1,
				"Объект %s лишний для тестового каталога.\nДопустимые имена: %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s.",
				filename,
				PROJECT_FILENAME,
				LEXER_ERROR_FILENAME, 
				LEXER_FILENAME,
				PARSER_ERROR_FILENAME,
				PARSER_FILENAME,
				ANALYZER_ERROR_FILENAME,
				ANALYZER_FILENAME,
				COMPILER_ERROR_FILENAME,
				OUT_ERROR_FILENAME,
				OUT_FILENAME,
				COMMENT_FILENAME
			);
		}
		if (error) {
			return error;
		}
	}
	return NULL;
}

static KError* check_for_integrity(KTest* test) {
	if (!test->project_file.is) {
		return k_error_register(1, "В тесте отсутствует файл %s. Нет входных данных для теста.", PROJECT_FILENAME);
	}
	if (
		!test->lexer_error_file.is && !test->lexer_file.is &&
		!test->parser_error_file.is && !test->parser_file.is &&
		!test->analyzer_error_file.is && !test->analyzer_file.is &&
		!test->compiler_error_file.is &&
		!test->out_error_file.is && !test->out_file.is
	) {
		return k_error_register(1, "В тесте нет ни одного шаблона для сравнения. Нет выходных данных для сравнения результата тестов.");
	}
	
	if (test->lexer_error_file.is) {
		if (test->lexer_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Результат работы лексера не может быть одновременно положительным и отрицательным.", LEXER_ERROR_FILENAME, LEXER_FILENAME);
		}
		if (test->parser_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то парсер уже не запускается.", LEXER_ERROR_FILENAME, PARSER_ERROR_FILENAME);
		}
		if (test->parser_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то парсер уже не запускается.", LEXER_ERROR_FILENAME, PARSER_FILENAME);
		}
		if (test->analyzer_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то анализатор уже не запускается.", LEXER_ERROR_FILENAME, ANALYZER_ERROR_FILENAME);
		}
		if (test->analyzer_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то анализатор уже не запускается.", LEXER_ERROR_FILENAME, ANALYZER_FILENAME);
		}
		if (test->compiler_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то компилятор уже не запускается.", LEXER_ERROR_FILENAME, COMPILER_ERROR_FILENAME);
		}
		if (test->out_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то программа не скомпилируется и не запустится.", LEXER_ERROR_FILENAME, OUT_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если лексер выдает ошибку, то программа не скомпилируется и не запустится.", LEXER_ERROR_FILENAME, OUT_FILENAME);
		}
	}
	
	if (test->parser_error_file.is) {
		if (test->parser_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Результат работы парсера не может быть одновременно положительным и отрицательным.", PARSER_ERROR_FILENAME, PARSER_FILENAME);
		}
		if (test->analyzer_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то анализатор уже не запускается.", PARSER_ERROR_FILENAME, ANALYZER_ERROR_FILENAME);
		}
		if (test->analyzer_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то анализатор уже не запускается.", PARSER_ERROR_FILENAME, ANALYZER_FILENAME);
		}
		if (test->compiler_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то компилятор уже не запускается.", PARSER_ERROR_FILENAME, COMPILER_ERROR_FILENAME);
		}
		if (test->out_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то программа не скомпилируется и не запустится.", PARSER_ERROR_FILENAME, OUT_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если парсер выдает ошибку, то программа не скомпилируется и не запустится.", PARSER_ERROR_FILENAME, OUT_FILENAME);
		}
	}
	
	if (test->analyzer_error_file.is) {
		if (test->analyzer_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Результат работы анализатора не может быть одновременно положительным и отрицательным.", ANALYZER_ERROR_FILENAME, ANALYZER_FILENAME);
		}
		if (test->compiler_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если анализатор выдает ошибку, то компилятор уже не запускается.", ANALYZER_ERROR_FILENAME, COMPILER_ERROR_FILENAME);
		}
		if (test->out_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если анализатор выдает ошибку, то программа не скомпилируется и не запустится.", ANALYZER_ERROR_FILENAME, OUT_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если анализатор выдает ошибку, то программа не скомпилируется и не запустится.", ANALYZER_ERROR_FILENAME, OUT_FILENAME);
		}
	}
	
	if (test->compiler_error_file.is) {
		if (test->out_error_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если компилятор выдает ошибку, то программа не скомпилируется и не запустится.", COMPILER_ERROR_FILENAME, OUT_ERROR_FILENAME);
		}
		if (test->out_file.is) {
			return k_error_register(1, "В тесте присутствуют одновременно %s и %s. Если компилятор выдает ошибку, то программа не скомпилируется и не запустится.", COMPILER_ERROR_FILENAME, OUT_FILENAME);
		}
	}
	
	return NULL;
}

KError* k_test_run(KTest* test, const char* dir) {
	size_t file_count = 0;
	char** files = k_file_create_directory_list(dir, &file_count);
	KError* error;
	error = check_for_test_directory(test, files, file_count);
	if (error) {
		return error;
	}
	
	error = check_for_integrity(test);
	if (error) {
		return error;
	}
	
	// TODO: "/" Получать через ОС, и вообще перенести в file_system или куда-то туда.
	char* path2 = k_string_concat(dir, "/");
	char* project_path = k_string_concat(path2, test->project_file.path);
	KStream* file = k_stream_create(project_path);
	free(project_path);
	KModule* module = k_module_create(test->project_file.path);
	
	{
		bool lexerResult = k_lexer_run(file, module);
		if (test->lexer_file.is) {
			if (!lexerResult) {
				return k_error_register(1, "Ошибка в лексере. Ожидалось, что лексер отработает нормально.");
			}
			
			char* testResult = k_token_sprint(module->token);
			char* gold_path = k_string_concat(path2, test->lexer_file.path);
			char* gold = k_file_load(gold_path);
			free(gold_path);
			
			// TODO: Сделать сравнение более подробно хотя бы номер первой строки, в которой не сопадает исходный файл с тестом.
			if (strcmp(testResult, gold)) {
				return k_error_register(1, "Ошибка в лексере. Выход теста не совпадает с ожидаемым.\n"
					"Эталон:\n%s\nВывод программы:\n%s",
					gold, testResult
				);
			}
		} else if (test->lexer_error_file.is) {
			if (!lexerResult) {
				// TODO: Возможно в файле сообщения об ошибке можно добавить формат для сравнения результатов.
				return NULL;
			} else {
				return k_error_register(1, "Ошибка в лексере. Ожидалось, что лексер вернет ошибку, но он отработал нормально.");
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
		bool parserResult = k_parser_run(module);
		if (test->parser_file.is) {
			if (!parserResult) {
				return k_error_register(1, "Ошибка в парсере. Ожидалось, что парсер отработает нормально.");
			}
		
			char* testResult = k_token_sprint(module->token);
			char* gold_path = k_string_concat(path2, test->parser_file.path);
			char* gold = k_file_load(gold_path);
			free(gold_path);
			
			// TODO: Сделать сравнение более подробно хотя бы номер первой строки, в которой не сопадает исходный файл с тестом.
			if (testResult != gold) {
				return k_error_register(1, "Ошибка в парсере. Выход теста не совпадает с ожидаемым.");
			}
			
		} else if (test->parser_error_file.is) {
			if (!parserResult) {
				// TODO: Возможно в файле сообщения об ошибке можно добавить формат для сравнения результатов.
				return NULL;
			} else {
				return k_error_register(1, "Ошибка в парсере. Ожидалось, что парсер вернет ошибку, но он отработал нормально.");
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
		bool generatorResult = k_generator_run(module);
		if (test->compiler_error_file.is) {
			if (!generatorResult) {
				// TODO: Возможно в файле сообщения об ошибке можно добавить формат для сравнения результатов.
				return NULL;
			} else {
				return k_error_register(1, "Ошибка в парсере. Ожидалось, что парсер вернет ошибку, но он отработал нормально.");
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
