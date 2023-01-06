/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_LEXER_H
#define KAR_LEXER_H

#include "core/stream.h"
#include "structure/module.h"

bool kar_lexer_run(KarStream* stream, KarModule* module);

#endif // KAR_LEXER_H
