/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/module.h"

#include <stdlib.h>
#include <string.h>

KModule* k_module_create(const char* name) {
	KModule* module = (KModule*)malloc(sizeof(KModule));
	
	size_t len = strlen(name);
	module->name = (char*)malloc((len + 1) * sizeof(char));
	strcpy(module->name, name);
	module->name[len] = 0;
	
	module->token = k_token_create();
	module->token->type = TOKEN_MODULE;
	
	return module;
}

void k_module_free(KModule* module) {
	free(module->name);
	free(module);
}
