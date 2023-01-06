/* Copyright © 2020,2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PARSER_H
#define KAR_PARSER_H

#include <stdbool.h>

#include "model/module.h"

bool kar_parser_run(KarModule* module);

#endif // KAR_PARSER_H
