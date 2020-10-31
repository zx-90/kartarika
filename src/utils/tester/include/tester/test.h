/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_TEST_H
#define KAR_TEST_H

#include <stdbool.h>

#include "core/error.h"

#define KAR_PROJECT_FILENAME "проект.кар"
#define KAR_LEXER_ERROR_FILENAME "лексер.ошибка"
#define KAR_LEXER_FILENAME "лексер"
#define KAR_PARSER_ERROR_FILENAME "парсер.ошибка"
#define KAR_PARSER_FILENAME "парсер"
#define KAR_ANALYZER_ERROR_FILENAME "анализатор.ошибка"
#define KAR_ANALYZER_FILENAME "анализатор"
#define KAR_COMPILER_ERROR_FILENAME "компилятор.ошибка"
#define KAR_OUT_ERROR_FILENAME "выход.ошибка"
#define KAR_OUT_FILENAME "выход"
#define KAR_COMMENT_FILENAME "коммент"

typedef struct {
	bool is;
	char* path;
} KarPathElement;

typedef struct {
	KarPathElement project_file;
	
	KarPathElement lexer_error_file;
	KarPathElement lexer_file;
	
	KarPathElement parser_error_file;
	KarPathElement parser_file;
	
	KarPathElement analyzer_error_file;
	KarPathElement analyzer_file;
	
	KarPathElement compiler_error_file;

	KarPathElement out_error_file;
	KarPathElement out_file;
	
	KarPathElement comment_file;
	
} KarTest;

void kar_path_element_init(KarPathElement* element);
void kar_path_element_clear(KarPathElement* element);
bool kar_path_element_set(KarPathElement* element, const char* path);

KarTest* kar_test_create();
void kar_test_free(KarTest* test);

KarError* kar_test_run(KarTest* test, const char* dir);

#endif // KAR_TEST_H
