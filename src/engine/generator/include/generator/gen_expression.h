/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_GEN_EXPRESSION_H
#define KAR_GEN_EXPRESSION_H

#include "model/token.h"
#include "model/project_error_list.h"
#include "llvm_data.h"

typedef struct {
	KarVartree* type;
	LLVMValueRef value;
} KarExpressionResult;

bool kar_expression_result_is_none(KarExpressionResult result);

KarExpressionResult kar_generate_calc_expression(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors);
bool kar_generate_expression(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors);

#endif // KAR_GEN_EXPRESSION_H
