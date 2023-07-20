/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_SECOND_LEXER_H
#define KAR_SECOND_LEXER_H

#include "model/module.h"

bool kar_second_lexer_run(KarModule* module, KarProjectErrorList* errors);

#endif // KAR_SECOND_LEXER_H
