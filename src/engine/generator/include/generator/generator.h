/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef GENERATOR_H
#define GENERATOR_H

#include "core/module.h"

#ifdef __cplusplus
extern "C" {
#endif

bool k_generator_run(KModule* module);

#ifdef __cplusplus
}
#endif

#endif // GENERATOR_H
