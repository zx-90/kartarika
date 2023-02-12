/* Copyright © 2020,2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_MODULE_H
#define KAR_MODULE_H

#include "core/array.h"
#include "token.h"
#include "project_error_list.h"

// TODO: Поле errors необходимо перенести в KarProject.
typedef struct {
	char* name;
	KarToken* token;
	KarProjectErrorList* errors;
} KarModule;

KarModule* kar_module_create(const char* name);
void kar_module_free(KarModule* module);

void kar_module_print_errors(KarModule* module);

#endif // KAR_MODULE_H
