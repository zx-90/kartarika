/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/llvm_data.h"

#include <llvm-c/Core.h>

KarLLVMData* kar_llvm_data_create(LLVMContextRef context, LLVMModuleRef module, LLVMBuilderRef builder) {
    KAR_CREATE(result, KarLLVMData);

	result->context = context;
    result->module = module;
    result->builder = builder;
    kar_llvm_data_functions_init(result);

	LLVMTypeRef pointerType = LLVMPointerType(LLVMInt8Type(), 0);
	LLVMTypeRef returnType = LLVMPointerType(LLVMInt8Type(), 0);

	LLVMTypeRef createPointerFuncType = LLVMFunctionType(returnType, &pointerType, 1, false);
	result->createPointer = LLVMAddFunction(module, "_kartarika_smart_pointer_create", createPointerFuncType);

	LLVMTypeRef addRefFuncType = LLVMFunctionType(LLVMVoidType(), &pointerType, 1, false);
	result->addRefPointer = LLVMAddFunction(module, "_kartarika_smart_pointer_add_ref", addRefFuncType);

	LLVMTypeRef freeFuncType = LLVMFunctionType(LLVMVoidType(), &pointerType, 1, false);
	result->freePointer = LLVMAddFunction(module, "_kartarika_smart_pointer_free", freeFuncType);

	LLVMTypeRef createStringFuncType = LLVMFunctionType(returnType, &pointerType, 1, false);
	result->createString = LLVMAddFunction(module, "_kartarika_library_string_create", createStringFuncType);

	LLVMTypeRef uncleanBoolFuncType = LLVMFunctionType(LLVMInt1Type(), &pointerType, 1, false);
	result->uncleanBool = LLVMAddFunction(module, "_kartarika_unclean_bool", uncleanBoolFuncType);

	return result;
}

void kar_llvm_data_free(KarLLVMData* data) {
    kar_llvm_data_functions_clear(data);
    KAR_FREE(data);
}

KAR_SET_CODE(llvm_data_functions, KarLLVMData, KarLLVMFunction, functions, kar_llvm_function_less, kar_llvm_function_equal, kar_llvm_function_free)

KarLLVMFunction* kar_llvm_data_get_function(KarLLVMData* llvmData, KarVartreeFunctionParams* params, KarVars* vars) {
	KarLLVMFunction* llvmFunc = kar_llvm_function_create(params);
	size_t num = kar_llvm_data_functions_add(llvmData, llvmFunc);
	if (num == (size_t)-1) {
		kar_llvm_function_free(llvmFunc);
		// TODO: Здесь надо искать быстрее, не просты перебором, так как массив отсортирован.
		for (size_t i = 0; i < kar_llvm_data_functions_count(llvmData); i++) {
			KarLLVMFunction* child = kar_llvm_data_functions_get(llvmData, i);
			if (kar_string_equal(params->issueName, child->params->issueName)) {
				return child;
			}
		}
		return NULL;
	}
	kar_llvm_function_init(llvmFunc, llvmData->module, vars);
	return llvmFunc;
}
