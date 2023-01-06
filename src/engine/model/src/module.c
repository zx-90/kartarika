/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/module.h"

#include <string.h>

#include "core/alloc.h"
#include "core/string.h"

KarModule* kar_module_create(const char* name) {
	KAR_CREATE(module, KarModule);
	
	module->name = kar_string_create_copy(name);
	module->token = kar_token_create();
	module->token->type = KAR_TOKEN_MODULE;
	kar_array_init(&module->errors);
	
	return module;
}

void kar_module_free(KarModule* module) {
	KAR_FREE(module->name);
	kar_token_free(module->token);
	kar_array_clear(&module->errors, (KarArrayFreeFn*)&kar_module_error_free);
	KAR_FREE(module);
}

void kar_module_print_errors(KarModule* module) {
	for (size_t i = 0; i < kar_module_error_get_count(module); ++i) {
		kar_module_error_print(module->name, kar_module_error_get(module, i));
	}
}
