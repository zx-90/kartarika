/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/llvm_function.h"

#include <llvm-c/Core.h>

KarLLVMFunction* kar_llvm_function_create(KarVartree* var) {
    KAR_CREATE(result, KarLLVMFunction);

	result->var = var;
    result->value = NULL;

    return result;
}

void kar_llvm_function_free(KarLLVMFunction* func) {
    KAR_FREE(func);
}

static LLVMTypeRef get_llvm_type(KarVartree* type, KarVars* vars) {
    if (type == NULL) {
        return LLVMVoidType();
    } else if (type == vars->standard.boolType) {
        return LLVMInt1Type();
    } else if (type == vars->standard.int8Type) {
        return LLVMInt8Type();
    } else if (type == vars->standard.int16Type) {
        return LLVMInt16Type();
    } else if (type == vars->standard.int32Type) {
        return LLVMInt32Type();
    } else if (type == vars->standard.int64Type) {
        return LLVMInt64Type();
    } else if (type == vars->standard.unsigned8Type) {
        return LLVMInt8Type();
    } else if (type == vars->standard.unsigned16Type) {
        return LLVMInt16Type();
    } else if (type == vars->standard.unsigned32Type) {
        return LLVMInt32Type();
    } else if (type == vars->standard.unsigned64Type) {
        return LLVMInt64Type();
    } else if (type == vars->standard.float32Type) {
        return LLVMFloatType();
    } else if (type == vars->standard.float64Type) {
        return LLVMDoubleType();
    } else if (type == vars->standard.stringType) {
        return LLVMPointerType(LLVMInt8Type(), 0);
    }
	return LLVMPointerType(LLVMInt8Type(), 0);
}

void kar_llvm_function_init(KarLLVMFunction* func, LLVMModuleRef module, KarVars* vars) {
    if (func->value != NULL) {
        return;
    }
	KarVartreeFunctionParams* params = kar_vartree_get_function_params(func->var);
	LLVMTypeRef returnType = get_llvm_type(params->returnType, vars);
	size_t arg_count = kar_vartree_args_count(func->var);
    KAR_CREATES(args, LLVMTypeRef, arg_count);
    for (size_t i = 0; i < arg_count; i++) {
		args[i] = get_llvm_type(kar_vartree_args_get(func->var, i), vars);
    }
    LLVMTypeRef puts_type = LLVMFunctionType(returnType, args, (unsigned int)arg_count, false);
	func->value = LLVMAddFunction(module, params->issueName, puts_type);
    KAR_FREE(args);
}

LLVMValueRef kar_llvm_function_get_ref(KarLLVMFunction* func) {
	return func->value;
}

bool kar_llvm_function_less(KarLLVMFunction* func1, KarLLVMFunction* func2) {
	return kar_vartree_less(func1->var, func2->var);
}

bool kar_llvm_function_equal(KarLLVMFunction* func1, KarLLVMFunction* func2) {
	return kar_vartree_equal(func1->var, func2->var);
}
