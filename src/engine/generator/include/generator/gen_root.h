/* Copyright © 2024 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_GEN_ROOT_H
#define KAR_GEN_ROOT_H

#include "model/token.h"
#include "model/project_error_list.h"
#include "llvm_data.h"

bool kar_generate_function(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors);
bool kar_generate_const(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors);

#endif // KAR_GEN_ROOT_H
