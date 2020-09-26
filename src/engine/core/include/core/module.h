/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_MODULE_H
#define KAR_MODULE_H

#include "token.h"

typedef struct {
	char* name;
	KarToken* token;
} KarModule;

KarModule* kar_module_create(const char* name);
void kar_module_free(KarModule* module);

#endif // KAR_MODULE_H
