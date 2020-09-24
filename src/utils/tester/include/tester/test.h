/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef TEST_H
#define TEST_H

#include <stdbool.h>

#include "core/error.h"

#define PROJECT_FILENAME "проект.кар"
#define LEXER_ERROR_FILENAME "лексер.ошибка"
#define LEXER_FILENAME "лексер"
#define PARSER_ERROR_FILENAME "парсер.ошибка"
#define PARSER_FILENAME "парсер"
#define ANALYZER_ERROR_FILENAME "анализатор.ошибка"
#define ANALYZER_FILENAME "анализатор"
#define COMPILER_ERROR_FILENAME "компилятор.ошибка"
#define OUT_ERROR_FILENAME "выход.ошибка"
#define OUT_FILENAME "выход"
#define COMMENT_FILENAME "коммент"

typedef struct {
	bool is;
	char* path;
} KPathElement;

typedef struct {
	KPathElement project_file;
	
	KPathElement lexer_error_file;
	KPathElement lexer_file;
	
	KPathElement parser_error_file;
	KPathElement parser_file;
	
	KPathElement analyzer_error_file;
	KPathElement analyzer_file;
	
	KPathElement compiler_error_file;

	KPathElement out_error_file;
	KPathElement out_file;
	
	KPathElement comment_file;
	
} KTest;

#ifdef __cplusplus
extern "C" {
#endif

void k_path_element_init(KPathElement* element);
bool k_path_element_set(KPathElement* element, const char* path);

KTest* k_test_create();
void k_test_free(KTest* test);

KError* k_test_run(KTest* test, const char* dir);

#ifdef __cplusplus
}
#endif

#endif // TEST_H
