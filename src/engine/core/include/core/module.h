/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef MODULE_H
#define MODULE_H

#include "token.h"

typedef struct {
	char* name;
	KToken* token;
} KModule;

#ifdef __cplusplus
extern "C" {
#endif

KModule* k_module_create(const char* name);
void k_module_free(KModule* module);

#ifdef __cplusplus
}
#endif

#endif // MODULE_H
