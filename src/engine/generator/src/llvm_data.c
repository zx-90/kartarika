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

	LLVMTypeRef boolType = LLVMInt1Type();
	LLVMTypeRef integer8Type = LLVMInt8Type();
	LLVMTypeRef integer16Type = LLVMInt16Type();
	LLVMTypeRef integer32Type = LLVMInt32Type();
	LLVMTypeRef integer64Type = LLVMInt64Type();
	LLVMTypeRef unsigned8Type = LLVMInt8Type();
	LLVMTypeRef unsigned16Type = LLVMInt16Type();
	LLVMTypeRef unsigned32Type = LLVMInt32Type();
	LLVMTypeRef unsigned64Type = LLVMInt64Type();
	LLVMTypeRef float32Type = LLVMFloatType();
	LLVMTypeRef float64Type = LLVMDoubleType();
	LLVMTypeRef pointerType = LLVMPointerType(LLVMInt8Type(), 0);


	LLVMTypeRef createPointerFuncType = LLVMFunctionType(pointerType, &pointerType, 1, false);
	result->createPointer = LLVMAddFunction(module, "_kartarika_smart_pointer_create", createPointerFuncType);

	LLVMTypeRef addRefFuncType = LLVMFunctionType(LLVMVoidType(), &pointerType, 1, false);
	result->addRefPointer = LLVMAddFunction(module, "_kartarika_smart_pointer_add_ref", addRefFuncType);

	LLVMTypeRef freeFuncType = LLVMFunctionType(LLVMVoidType(), &pointerType, 1, false);
	result->freePointer = LLVMAddFunction(module, "_kartarika_smart_pointer_free", freeFuncType);

	LLVMTypeRef addStringFuncIn[] = {pointerType, pointerType};
	LLVMTypeRef addStringFuncType = LLVMFunctionType(pointerType, addStringFuncIn, 2, false);
	result->addString = LLVMAddFunction(module, "_kartarika_library_string_sum", addStringFuncType);

	LLVMTypeRef isEqualStringFuncType = LLVMFunctionType(boolType, addStringFuncIn, 2, false);
	result->isEqualString = LLVMAddFunction(module, "_kartarika_library_string_is_equal", isEqualStringFuncType);

	LLVMTypeRef createStringFuncType = LLVMFunctionType(pointerType, &pointerType, 1, false);
	result->createString = LLVMAddFunction(module, "_kartarika_library_string_create", createStringFuncType);

	LLVMTypeRef cleanBoolFuncType = LLVMFunctionType(pointerType, &boolType, 1, false);
	result->cleanBool = LLVMAddFunction(module, "_kartarika_library_clean_bool", cleanBoolFuncType);

	LLVMTypeRef cleanInteger8FuncType = LLVMFunctionType(pointerType, &integer8Type, 1, false);
	result->cleanInteger8 = LLVMAddFunction(module, "_kartarika_library_clean_integer8", cleanInteger8FuncType);

	LLVMTypeRef cleanInteger16FuncType = LLVMFunctionType(pointerType, &integer16Type, 1, false);
	result->cleanInteger16 = LLVMAddFunction(module, "_kartarika_library_clean_integer16", cleanInteger16FuncType);

	LLVMTypeRef cleanInteger32FuncType = LLVMFunctionType(pointerType, &integer32Type, 1, false);
	result->cleanInteger32 = LLVMAddFunction(module, "_kartarika_library_clean_integer32", cleanInteger32FuncType);

	LLVMTypeRef cleanInteger64FuncType = LLVMFunctionType(pointerType, &integer64Type, 1, false);
	result->cleanInteger64 = LLVMAddFunction(module, "_kartarika_library_clean_integer64", cleanInteger64FuncType);

	LLVMTypeRef cleanUnsigned8FuncType = LLVMFunctionType(pointerType, &unsigned8Type, 1, false);
	result->cleanUnsigned8 = LLVMAddFunction(module, "_kartarika_library_clean_unsigned8", cleanUnsigned8FuncType);

	LLVMTypeRef cleanUnsigned16FuncType = LLVMFunctionType(pointerType, &unsigned16Type, 1, false);
	result->cleanUnsigned16 = LLVMAddFunction(module, "_kartarika_library_clean_unsigned16", cleanUnsigned16FuncType);

	LLVMTypeRef cleanUnsigned32FuncType = LLVMFunctionType(pointerType, &unsigned32Type, 1, false);
	result->cleanUnsigned32 = LLVMAddFunction(module, "_kartarika_library_clean_unsigned32", cleanUnsigned32FuncType);

	LLVMTypeRef cleanUnsigned64FuncType = LLVMFunctionType(pointerType, &unsigned64Type, 1, false);
	result->cleanUnsigned64 = LLVMAddFunction(module, "_kartarika_library_clean_unsigned64", cleanUnsigned64FuncType);

	LLVMTypeRef cleanFloat32FuncType = LLVMFunctionType(pointerType, &float32Type, 1, false);
	result->cleanFloat32 = LLVMAddFunction(module, "_kartarika_library_clean_float32", cleanFloat32FuncType);

	LLVMTypeRef cleanFloat64FuncType = LLVMFunctionType(pointerType, &float64Type, 1, false);
	result->cleanFloat64 = LLVMAddFunction(module, "_kartarika_library_clean_float64", cleanFloat64FuncType);

	LLVMTypeRef cleanStringFuncType = LLVMFunctionType(pointerType, &pointerType, 1, false);
	result->cleanString = LLVMAddFunction(module, "_kartarika_library_clean_string", cleanStringFuncType);

	LLVMTypeRef uncleanBoolFuncType = LLVMFunctionType(boolType, &pointerType, 1, false);
	result->uncleanBool = LLVMAddFunction(module, "_kartarika_unclean_bool", uncleanBoolFuncType);

	LLVMTypeRef uncleanInteger8FuncType = LLVMFunctionType(integer8Type, &pointerType, 1, false);
	result->uncleanInteger8 = LLVMAddFunction(module, "_kartarika_unclean_integer8", uncleanInteger8FuncType);

	LLVMTypeRef uncleanInteger16FuncType = LLVMFunctionType(integer16Type, &pointerType, 1, false);
	result->uncleanInteger16 = LLVMAddFunction(module, "_kartarika_unclean_integer16", uncleanInteger16FuncType);

	LLVMTypeRef uncleanInteger32FuncType = LLVMFunctionType(integer32Type, &pointerType, 1, false);
	result->uncleanInteger32 = LLVMAddFunction(module, "_kartarika_unclean_integer32", uncleanInteger32FuncType);

	LLVMTypeRef uncleanInteger64FuncType = LLVMFunctionType(integer64Type, &pointerType, 1, false);
	result->uncleanInteger64 = LLVMAddFunction(module, "_kartarika_unclean_integer64", uncleanInteger64FuncType);

	LLVMTypeRef uncleanUnsigned8FuncType = LLVMFunctionType(unsigned8Type, &pointerType, 1, false);
	result->uncleanUnsigned8 = LLVMAddFunction(module, "_kartarika_unclean_unsigned8", uncleanUnsigned8FuncType);

	LLVMTypeRef uncleanUnsigned16FuncType = LLVMFunctionType(unsigned16Type, &pointerType, 1, false);
	result->uncleanUnsigned16 = LLVMAddFunction(module, "_kartarika_unclean_unsigned16", uncleanUnsigned16FuncType);

	LLVMTypeRef uncleanUnsigned32FuncType = LLVMFunctionType(unsigned32Type, &pointerType, 1, false);
	result->uncleanUnsigned32 = LLVMAddFunction(module, "_kartarika_unclean_unsigned32", uncleanUnsigned32FuncType);

	LLVMTypeRef uncleanUnsigned64FuncType = LLVMFunctionType(unsigned64Type, &pointerType, 1, false);
	result->uncleanUnsigned64 = LLVMAddFunction(module, "_kartarika_unclean_unsigned64", uncleanUnsigned64FuncType);

	LLVMTypeRef uncleanFloat32FuncType = LLVMFunctionType(float32Type, &pointerType, 1, false);
	result->uncleanFloat32 = LLVMAddFunction(module, "_kartarika_unclean_float32", uncleanFloat32FuncType);

	LLVMTypeRef uncleanFloat64FuncType = LLVMFunctionType(float64Type, &pointerType, 1, false);
	result->uncleanFloat64 = LLVMAddFunction(module, "_kartarika_unclean_float64", uncleanFloat64FuncType);

	LLVMTypeRef uncleanStringFuncType = LLVMFunctionType(pointerType, &pointerType, 1, false);
	result->uncleanString = LLVMAddFunction(module, "_kartarika_unclean_string", uncleanStringFuncType);

	result->counter = 0;

	return result;
}

void kar_llvm_data_free(KarLLVMData* data) {
    kar_llvm_data_functions_clear(data);
    KAR_FREE(data);
}

KAR_SET_CODE(llvm_data_functions, KarLLVMData, KarLLVMFunction, functions, kar_llvm_function_less, kar_llvm_function_equal, kar_llvm_function_free)

KarLLVMFunction* kar_llvm_data_get_function(KarLLVMData* llvmData, KarVartree* func, KarVars* vars) {
	KarLLVMFunction* llvmFunc = kar_llvm_function_create(func);
	size_t num = kar_llvm_data_functions_add(llvmData, llvmFunc);
	if (num == (size_t)-1) {
		kar_llvm_function_free(llvmFunc);
		KarVartreeFunctionParams* params = kar_vartree_get_function_params(func);
		// TODO: Здесь надо искать быстрее, не просты перебором, так как массив отсортирован.
		for (size_t i = 0; i < kar_llvm_data_functions_count(llvmData); i++) {
			KarLLVMFunction* child = kar_llvm_data_functions_get(llvmData, i);
			KarVartreeFunctionParams* child_params = kar_vartree_get_function_params(child->var);
			if (kar_string_equal(params->issueName, child_params->issueName)) {
				return child;
			}
		}
		return NULL;
	}
	kar_llvm_function_init(llvmFunc, llvmData->module, vars);
	return llvmFunc;
}
