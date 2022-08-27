/* Copyright © 2022 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/token.h"

bool kar_parser_is_expression(KarToken* token)
{
	// TODO: Написать функцию и добавить тесты. Проверка на то,
	//       является ли токен вместе со всеми потомками корректным выражением.
	KarTokenType type = token->type;
	return type != KAR_TOKEN_MODIFIER_STAT;
}