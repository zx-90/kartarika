/* Copyright © 2024 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/gen_root.h"

#include <llvm-c/Core.h>

#include "model/vartree_var.h"

#include "generator/gen_expression.h"
#include "generator/gen_algorithm.h"

bool kar_generate_function(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_METHOD) {
		return false;
	}
	if (kar_string_equal(token->str, "Запустить")) {
		LLVMTypeRef func_type = LLVMFunctionType(LLVMVoidType(), NULL, 0, false);
		LLVMValueRef main_func = LLVMAddFunction(llvmData->module, "main", func_type);
		LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main_func, "entry");
		LLVMPositionBuilderAtEnd(llvmData->builder, entry);

		KarToken* body = kar_token_child_get_last(token, 0);

		kar_local_stack_block_insert(vars->locals, kar_local_block_create(), 0);
		if (!kar_generate_algorithm(body, llvmData, module, vars, errors)) {
			return false;
		}
		kar_local_stack_block_erase(vars->locals, 0);
		LLVMBuildRetVoid(llvmData->builder);
	} else {
		// Далее здесь необходимо дописать поддержку других методов.
		kar_project_error_list_create_add(errors, module->name, &token->cursor, 1, "Методы не поддерживаются. Поддерживается только метод \"Запустить\".");
		return false;
	}
	return true;
}

bool kar_generate_const(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	// TODO: Нужны ли динамические константы? может быть лучше отменить стат/динамическое для констант.
	uint8_t statMod = 0xFF;
	uint8_t areaMod = 0xFF;
	KarToken* modificators = kar_token_child_get(token, 0);
	for (size_t i = 0; i < kar_token_child_count(modificators); i++) {
		KarToken* modificatorToken = kar_token_child_get(modificators, i);
		switch (modificatorToken->type) {
		case KAR_TOKEN_MODIFIER_STAT: {
			if (statMod != 0xFF) {
				kar_project_error_list_create_add(errors, module->name, &modificatorToken->cursor, 1, "Двойной модификатор статичности.");
				return false;
			}
			statMod = VAR_STATIC;
			break;
		}
		case KAR_TOKEN_VAR_MODIFIER_DYNAMIC: {
			if (statMod != 0xFF) {
				kar_project_error_list_create_add(errors, module->name, &modificatorToken->cursor, 1, "Двойной модификатор статичности.");
				return false;
			}
			statMod = VAR_DYNAMIC;
			break;
		}
		case KAR_TOKEN_VAR_MODIFIER_PRIVATE: {
			if (areaMod != 0xFF) {
				kar_project_error_list_create_add(errors, module->name, &modificatorToken->cursor, 1, "Двойной модификатор области видимости.");
				return false;
			}
			areaMod = VAR_PRIVATE;
			break;
		}
		case KAR_TOKEN_VAR_MODIFIER_PROTECTED: {
			if (areaMod != 0xFF) {
				kar_project_error_list_create_add(errors, module->name, &modificatorToken->cursor, 1, "Двойной модификатор области видимости.");
				return false;
			}
			areaMod = VAR_PROTECTED;
			break;
		}
		case KAR_TOKEN_VAR_MODIFIER_PUBLIC: {
			if (areaMod != 0xFF) {
				kar_project_error_list_create_add(errors, module->name, &modificatorToken->cursor, 1, "Двойной модификатор области видимости.");
				return false;
			}
			areaMod = VAR_PUBLIC;
			break;
		}
		default: {
			kar_project_error_list_create_add(errors, module->name, &modificatorToken->cursor, 1, "Неизвестный модификатор константы.");
			return false;
		}
		}
	}
	if (statMod == 0xFF) {
		statMod = VAR_DYNAMIC;
	}
	if (areaMod == 0xFF) {
		areaMod = VAR_PRIVATE;
	}

	if (statMod != VAR_STATIC) {
		kar_project_error_list_create_add(errors, module->name, &token->cursor, 1, "Корневые константы должны быть только статическими.");
		return false;
	}

	KarToken* valueToken = kar_token_child_get(token, 1);
	KarToken* expressionToken = kar_token_child_get(valueToken, 0);
	KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, module, vars, errors);
	if (kar_expression_result_is_none(result)) {
		return false;
	}

	kar_vartree_create_const(module, token->str, statMod | areaMod, result.type, (void*)result.value);

	//kar_project_error_list_create_add(errors, module->name, &token->cursor, 1, "Функция обработки констант не реализована.");
	return true;
}
