/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_MODULE_H
#define KAR_MODULE_H

#include "core/array.h"
#include "token.h"
#include "module_error.h"

typedef struct {
	char* name;
	KarToken* token;
	KarArray errors;
} KarModule;

#define kar_module_error_get_count(module) ((module)->errors.count)
#define kar_module_error_get(module, num) ((KarModuleError*)(module)->errors.items[(num)])

KarModule* kar_module_create(const char* name);
void kar_module_free(KarModule* module);

void kar_module_print_errors(KarModule* module);

#endif // KAR_MODULE_H
