/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/module.h"

#include <string.h>

#include "core/alloc.h"

KModule* k_module_create(const char* name) {
	K_CREATE(module, KModule);
	
	size_t len = strlen(name);
	K_ALLOCS(module->name, char, len + 1);
	strcpy(module->name, name);
	module->name[len] = 0;
	
	module->token = k_token_create();
	module->token->type = TOKEN_MODULE;
	
	return module;
}

void k_module_free(KModule* module) {
	K_FREE(module->name);
	K_FREE(module);
}
