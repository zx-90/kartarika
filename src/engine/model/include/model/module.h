/* Copyright © 2020,2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_MODULE_H
#define KAR_MODULE_H

#include "core/array.h"
#include "core/string.h"
#include "token.h"
#include "project_error_list.h"

typedef struct {
	KarString* name;
	KarToken* token;
} KarModule;

KarModule* kar_module_create(const KarString* name);
void kar_module_free(KarModule* module);

KarString* kar_module_get_full_name(KarModule* module);

#endif // KAR_MODULE_H
