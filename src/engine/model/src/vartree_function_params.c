/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/vartree_function_params.h"

#include "core/string_builder.h"
#include "model/vartree.h"

static void(*nullFree)(KarVartree* item) = NULL;

KarVartreeFunctionParams* kar_vartree_function_create(uint8_t modificators, const KarString* issueName, KarVartree** args, size_t args_count, KarVartree* return_type) {
    KAR_CREATE(value, KarVartreeFunctionParams);

	value->modificators = modificators;
    value->issueName = kar_string_create(issueName);
    kar_vartree_function_params_args_init(value);
    for (size_t i = 0; i < args_count; i++) {
        kar_vartree_function_params_args_add(value, args[i]);
    }
    value->returnType = return_type;

    return value;
}

void kar_vartree_function_free(void* ptr) {
    KarVartreeFunctionParams* value = (KarVartreeFunctionParams*)ptr;
    if (value->issueName != NULL) {
        KAR_FREE(value->issueName);
    }
    kar_vartree_function_params_args_clear(value);

    KAR_FREE(value);
}

KAR_ARRAY_CODE(vartree_function_params_args, KarVartreeFunctionParams, KarVartree, args, nullFree)

KarString* kar_vartree_create_full_function_name(const KarString* name, KarVartree** args, size_t args_count) {
	if (name == NULL) {
		return NULL;
	}
    KarStringBuilder builder;
    kar_string_builder_init(&builder);

    kar_string_builder_push_string(&builder, name);
    kar_string_builder_push_string(&builder, "(");
    for (size_t i = 0; i < args_count; i++) {
        if (i != 0) {
            kar_string_builder_push_string(&builder, ",");
        }
        KarString* argName = kar_vartree_create_full_path(args[i]);
        kar_string_builder_push_string(&builder, argName);
        KAR_FREE(argName);
    }
    kar_string_builder_push_string(&builder, ")");

    return kar_string_builder_clear_get(&builder);
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

