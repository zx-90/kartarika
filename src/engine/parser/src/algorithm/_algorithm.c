/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"
#include "core/module_error.h"

bool kar_parser_make_return(KarToken* token);

bool kar_parser_parse_algorithm(KarToken* token) {
	bool b = true;
	b = b && kar_parser_make_return(token);
	return b;
}
