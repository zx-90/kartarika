/* Copyright © 2020,2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/module.h"

#include <string.h>

#include "core/alloc.h"
#include "core/string.h"

KarModule* kar_module_create(const KarString* name) {
	KAR_CREATE(module, KarModule);
	
	module->name = kar_string_create(name);
	module->token = kar_token_create();
	module->token->type = KAR_TOKEN_MODULE;
	module->errors = kar_project_error_list_create();
	
	return module;
}

void kar_module_free(KarModule* module) {
	kar_string_free(module->name);
	kar_token_free(module->token);
	kar_project_error_list_free(module->errors);
	KAR_FREE(module);
}

void kar_module_print_errors(KarModule* module) {
	for (size_t i = 0; i < kar_project_error_list_count(module->errors); ++i) {
		kar_project_error_print(module->name, kar_project_error_list_get(module->errors, i));
	}
}
