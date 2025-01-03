/* Copyright © 2025 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_VARTREE_TEMPLATE_H
#define KAR_VARTREE_TEMPLATE_H

#include "model/vartree.h"

KarVartree* kar_vartree_create_unclean(KarVartree* parent, const KarString* name);
KarVartree* kar_vartree_create_unclean_class(KarVartree* parent, KarVartree* type);

#endif // KAR_VARTREE_TEMPLATE_H
