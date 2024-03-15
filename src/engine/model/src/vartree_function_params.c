/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree_function_params.h"

#include "core/string_builder.h"
#include "model/vartree.h"

KarVartreeFunctionParams* kar_vartree_function_create(uint8_t modificators, const KarString* issueName, KarVartree* return_type) {
    KAR_CREATE(value, KarVartreeFunctionParams);

	value->modificators = modificators;
    value->issueName = kar_string_create(issueName);
    value->returnType = return_type;

    return value;
}

void kar_vartree_function_free(void* ptr) {
    KarVartreeFunctionParams* value = (KarVartreeFunctionParams*)ptr;
    if (value->issueName != NULL) {
        KAR_FREE(value->issueName);
    }
    KAR_FREE(value);
}

bool kar_vartree_function_is_static(uint8_t modificators) {
	return (modificators & 1) == 0;
}

bool kar_vartree_function_is_dynamic(uint8_t modificators) {
	return (modificators & 1) == 1;
}

bool kar_vartree_function_is_private(uint8_t modificators) {
	return (modificators & (3 << 1)) == 0 << 1;
}

bool kar_vartree_function_is_protected(uint8_t modificators) {
	return (modificators & (3 << 1)) == 1 << 1;
}

bool kar_vartree_function_is_public(uint8_t modificators) {
	return (modificators & (3 << 1)) == 2 << 1;
}

bool kar_vartree_function_is_finalized(uint8_t modificators) {
	return (modificators & (1 << 3)) == 0 << 3;
}

bool kar_vartree_function_is_heritable(uint8_t modificators) {
	return (modificators & (1 << 3)) == 1 << 3;
}

bool kar_vartree_function_is_overloaded(uint8_t modificators) {
	return (modificators & (1 << 4)) == 1 << 4;
}

