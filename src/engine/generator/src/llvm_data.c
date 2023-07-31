/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/llvm_data.h"

KarLLVMData* kar_llvm_data_create(LLVMModuleRef module, LLVMBuilderRef builder) {
    KAR_CREATE(result, KarLLVMData);

    result->module = module;
    result->builder = builder;
    kar_llvm_data_functions_init(result);

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
