/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_LLVM_DATA_H
#define KAR_LLVM_DATA_H

#include "core/set.h"

#include "llvm_function.h"

typedef struct {
	LLVMContextRef context;
	LLVMModuleRef module;
    LLVMBuilderRef builder;
    KAR_SET_STRUCT(KarLLVMFunction) functions;

	LLVMValueRef createPointer;
	LLVMValueRef addRefPointer;
	LLVMValueRef freePointer;

	LLVMValueRef createString;
	LLVMValueRef addString;
	LLVMValueRef isEqualString;

	LLVMValueRef cleanBool;
	LLVMValueRef cleanInteger8;
	LLVMValueRef cleanInteger16;
	LLVMValueRef cleanInteger32;
	LLVMValueRef cleanInteger64;
	LLVMValueRef cleanUnsigned8;
	LLVMValueRef cleanUnsigned16;
	LLVMValueRef cleanUnsigned32;
	LLVMValueRef cleanUnsigned64;
	LLVMValueRef cleanFloat32;
	LLVMValueRef cleanFloat64;
	LLVMValueRef cleanString;

	LLVMValueRef uncleanBool;
	LLVMValueRef uncleanInteger8;
	LLVMValueRef uncleanInteger16;
	LLVMValueRef uncleanInteger32;
	LLVMValueRef uncleanInteger64;
	LLVMValueRef uncleanUnsigned8;
	LLVMValueRef uncleanUnsigned16;
	LLVMValueRef uncleanUnsigned32;
	LLVMValueRef uncleanUnsigned64;
	LLVMValueRef uncleanFloat32;
	LLVMValueRef uncleanFloat64;
	LLVMValueRef uncleanString;

	size_t counter;
} KarLLVMData;

KarLLVMData* kar_llvm_data_create(LLVMContextRef context, LLVMModuleRef module, LLVMBuilderRef builder);
void kar_llvm_data_free(KarLLVMData* data);

KAR_SET_HEADER(llvm_data_functions, KarLLVMData, KarLLVMFunction)

KarLLVMFunction* kar_llvm_data_get_function(KarLLVMData* llvmData, KarVartree* func, KarVars* vars);

#endif // KAR_LLVM_DATA_H
