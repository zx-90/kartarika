/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_LLVM_FUNCTION_H
#define KAR_LLVM_FUNCTION_H

#include <llvm-c/BitWriter.h>

#include "model/vars.h"
#include "model/vartree_function_params.h"

typedef struct {
    KarVartreeFunctionParams* params;
    LLVMValueRef value;
} KarLLVMFunction;

KarLLVMFunction* kar_llvm_function_create(KarVartreeFunctionParams* params);
void kar_llvm_function_free(KarLLVMFunction* func);

void kar_llvm_function_init(KarLLVMFunction* func, LLVMModuleRef module, KarVars *vars);

LLVMValueRef kar_llvm_function_get_ref(KarLLVMFunction* func);

bool kar_llvm_function_less(KarLLVMFunction* func1, KarLLVMFunction* func2);
bool kar_llvm_function_equal(KarLLVMFunction* func1, KarLLVMFunction* func2);

#endif // KAR_LLVM_FUNCTION_H
