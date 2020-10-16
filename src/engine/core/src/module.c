/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/module.h"

#include <string.h>

#include "core/alloc.h"

KarModule* kar_module_create(const char* name) {
	KAR_CREATE(module, KarModule);
	
	size_t len = strlen(name);
	KAR_ALLOCS(module->name, char, len + 1);
	strcpy(module->name, name);
	module->name[len] = 0;
	
	module->token = kar_token_create();
	module->token->type = KAR_TOKEN_MODULE;
	
	return module;
}

void kar_module_free(KarModule* module) {
	KAR_FREE(module->name);
	kar_token_free(module->token);
	KAR_FREE(module);
}
