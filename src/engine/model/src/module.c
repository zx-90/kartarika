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
	
	return module;
}

void kar_module_free(KarModule* module) {
	kar_string_free(module->name);
	kar_token_free(module->token);
	KAR_FREE(module);
}

KarString* kar_module_get_full_name(KarModule* module) {
    // TODO: Вычислять полное имя модуля с полным путём.
    return module->name;
}
