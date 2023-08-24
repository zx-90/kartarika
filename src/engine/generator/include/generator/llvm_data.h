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

	LLVMValueRef uncleanBool;
} KarLLVMData;

KarLLVMData* kar_llvm_data_create(LLVMContextRef context, LLVMModuleRef module, LLVMBuilderRef builder);
void kar_llvm_data_free(KarLLVMData* data);

KAR_SET_HEADER(llvm_data_functions, KarLLVMData, KarLLVMFunction)

KarLLVMFunction* kar_llvm_data_get_function(KarLLVMData* llvmData, KarVartreeFunctionParams* params, KarVars* vars);

#endif // KAR_LLVM_DATA_H
