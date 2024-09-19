/* Copyright © 2024 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/gen_algorithm.h"

#include "generator/gen_expression.h"

#include <llvm-c/Core.h>

static bool generate_algorithm(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors);

static bool generate_declaration(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* varNameToken = kar_token_child_get(token, 0);
	if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
		kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Правая часть объявления переменной имеет не корректное имя.");
		return false;
	}
	KarLocalBlock* block = kar_local_stack_block_get(vars->locals, 0);
	if (kar_local_block_get_var_by_name(block, varNameToken->str) != NULL) {
		kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Переменная с таким именем уже существует.");
		return false;
	}
	KarToken* expressionToken = kar_token_child_get(token, 1);
	KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, moduleName, vars, errors);
	if (kar_expression_result_is_none(result)) {
		return false;
	}
	KarLocalVar* var = kar_local_var_create(varNameToken->str, result.type, result.value);
	kar_local_block_var_add(block, var);
	return true;
}

static bool generate_const_declaration(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* varNameToken = kar_token_child_get(token, 0);
	if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
		kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Правая часть объявления переменной имеет не корректное имя.");
		return false;
	}
	KarLocalBlock* block = kar_local_stack_block_get(vars->locals, 0);
	if (kar_local_block_get_var_by_name(block, varNameToken->str) != NULL) {
		kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Переменная с таким именем уже существует.");
		return false;
	}
	KarToken* expressionToken = kar_token_child_get(token, 1);
	KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, moduleName, vars, errors);
	if (kar_expression_result_is_none(result)) {
		return false;
	}
	KarLocalVar* var = kar_local_var_create_const(varNameToken->str, result.type, result.value);
	kar_local_block_var_add(block, var);
	return true;
}

static bool generate_assign(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* varNameToken = kar_token_child_get(token, 0);
	if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
		kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Правая часть  имеет не корректное имя.");
		return false;
	}
	KarLocalBlock* block = kar_local_stack_block_get(vars->locals, 0);
	KarLocalVar* var = kar_local_block_get_var_by_name(block, varNameToken->str);
	if (var == NULL) {
		kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Переменной с таким именем не существует.");
		return false;
	}
	if (var->is_const) {
		kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Попытка присвоения константе нового значения.");
		return false;
	}
	KarToken* expressionToken = kar_token_child_get(token, 1);
	KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, moduleName, vars, errors);
	if (kar_expression_result_is_none(result)) {
		return false;
	}
	KarExpressionResult varExpr = {var->type, var->value};
	if (var->type != result.type && !kar_expression_cast_type(&result, &varExpr, llvmData, vars)) {
		KarString* varType = kar_vartree_create_full_path(var->type);
		KarString* expressionType = kar_vartree_create_full_path(result.type);
		KarString* errorStr = kar_string_create_format(
			"Попытка присвоить переменной %s с типом %s несовместимый с ним тип %s",
			var->name, varType, expressionType
		);
		KAR_FREE(expressionType);
		KAR_FREE(varType);
		kar_project_error_list_create_add(errors, moduleName, &expressionToken->cursor, 1, errorStr);
		KAR_FREE(errorStr);
		return false;
	}
	var->value = result.value;
	return true;
}

static bool generate_block_body(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_BLOCK_BODY) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутренняя ошибка. Тип потомка блока не является его телом.");
		return false;
	}
	kar_local_stack_block_insert(vars->locals, kar_local_block_create(), 0);
	if (!kar_generate_algorithm(token, llvmData, moduleName, vars, errors)) {
		kar_local_stack_block_erase(vars->locals, 0);
		return false;
	}
	kar_local_stack_block_erase(vars->locals, 0);
	return true;
}

static bool generate_block(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	if (kar_token_child_count(token) != 1) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутренняя ошибка. Количество потомков блока должно быть равно 1.");
		return false;
	}
	KarToken* block_body = kar_token_child_get(token, 0);
	return (generate_block_body(block_body, llvmData, moduleName, vars, errors));
}

static bool generate_clean(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* uncleanedToken = kar_token_child_get(token, 0);
	KarExpressionResult uncleaned = kar_generate_calc_expression(uncleanedToken, llvmData, moduleName, vars, errors);
	if (kar_expression_result_is_none(uncleaned)) {
		return false;
	}
	if (uncleaned.type->type != KAR_VARTYPE_UNCLEAN_CLASS) {
		kar_project_error_list_create_add(errors, moduleName, &uncleanedToken->cursor, 1, "Переменная не является неопределённостью.");
		return false;
	}

	KarVartree* function = kar_vars_find_child(vars, vars->standard.unclean, "ПустойЛи", NULL, 0);
	KarLLVMFunction* llvmFunc = kar_llvm_data_get_function(llvmData, function, vars);
	LLVMValueRef expressionValue = LLVMBuildCall(llvmData->builder, kar_llvm_function_get_ref(llvmFunc), (LLVMValueRef*)&uncleaned.value, 1, "var");

	LLVMValueRef theFunction = LLVMGetBasicBlockParent(LLVMGetInsertBlock(llvmData->builder));
	KarString* thenString = kar_string_create_format("then%lu", llvmData->counter);
	LLVMBasicBlockRef thenBlock = LLVMAppendBasicBlock(theFunction, thenString);
	KAR_FREE(thenString);
	KarString* elseString = kar_string_create_format("else%lu", llvmData->counter);
	LLVMBasicBlockRef elseBlock = LLVMAppendBasicBlock(theFunction, elseString);
	KAR_FREE(elseString);
	KarString* mergeString = kar_string_create_format("merge%lu", llvmData->counter);
	LLVMBasicBlockRef mergeBlock = LLVMAppendBasicBlock(theFunction, mergeString);
	KAR_FREE(mergeString);
	llvmData->counter++;
	LLVMBuildCondBr(llvmData->builder, expressionValue, thenBlock, elseBlock);

	LLVMPositionBuilderAtEnd(llvmData->builder, thenBlock);
	if (kar_token_child_count(token) > 3) {
		KarToken* elseToken = kar_token_child_get(token, 3);
		if (!generate_block_body(elseToken, llvmData, moduleName, vars, errors)) {
			return false;
		}
	}

	LLVMBuildBr(llvmData->builder, mergeBlock);
	thenBlock = LLVMGetInsertBlock(llvmData->builder);

	LLVMPositionBuilderAtEnd(llvmData->builder, elseBlock);

	KarToken* thenToken = kar_token_child_get(token, 2);
	if (thenToken->type != KAR_TOKEN_BLOCK_BODY) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутренняя ошибка. Тип потомка блока не является его телом.");
		return false;
	}

	KarString* cleanName;
	KarToken* tokenName = kar_token_child_get(token, 1);
	if (tokenName->type == KAR_TOKEN_VAL_NULL) {
		if (uncleanedToken->type == KAR_TOKEN_IDENTIFIER) {
			cleanName = uncleanedToken->str;
		} else {
			kar_project_error_list_create_add(errors, moduleName, &tokenName->cursor, 1, "Раскрываемое выражение должно иметь идентификатор после ключевого слова \"как\".");
			return false;
		}
	} else if (tokenName->type == KAR_TOKEN_IDENTIFIER) {
		cleanName = tokenName->str;
	} else {
		kar_project_error_list_create_add(errors, moduleName, &tokenName->cursor, 1, "Выражение после ключевого слова \"как\" должно быть идентификатором.");
		return false;
	}
	KarVartree* cleanType = kar_vartree_args_get(uncleaned.type, 0);
	LLVMValueRef cleanValue = LLVMBuildCall(llvmData->builder, getLLVMCleanFunctionByType(cleanType->type, llvmData), (LLVMValueRef*)&uncleaned.value, 1, "");

	kar_local_stack_block_insert(vars->locals, kar_local_block_create(), 0);
	KarLocalBlock* subblock = kar_local_stack_block_get(vars->locals, 0);
	KarLocalVar* var = kar_local_var_create(cleanName, cleanType, cleanValue);
	kar_local_block_var_add(subblock, var);

	if (!kar_generate_algorithm(thenToken, llvmData, moduleName, vars, errors)) {
		kar_local_stack_block_erase(vars->locals, 0);
		return false;
	}
	kar_local_stack_block_erase(vars->locals, 0);

	LLVMBuildBr(llvmData->builder, mergeBlock);
	elseBlock = LLVMGetInsertBlock(llvmData->builder);

	LLVMPositionBuilderAtEnd(llvmData->builder, mergeBlock);
	LLVMValueRef phi = LLVMBuildPhi(llvmData->builder, LLVMVoidType(), "ph");
	LLVMValueRef phi_res = LLVMConstInt(LLVMInt1Type(), 0, 0);
	LLVMValueRef phi_res2 = LLVMConstInt(LLVMInt1Type(), 0, 0);
	LLVMAddIncoming(phi, &phi_res, &thenBlock, 1);
	LLVMAddIncoming(phi, &phi_res2, &elseBlock, 1);

	return true;
}

static bool generate_one_if(KarToken* token, size_t i, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* conditionToken = kar_token_child_get(token, i);
	KarExpressionResult condition = kar_generate_calc_expression(conditionToken, llvmData, moduleName, vars, errors);
	if (kar_expression_result_is_none(condition)) {
		return false;
	}
	if (condition.type->type != KAR_VARTYPE_BOOL) {
		kar_project_error_list_create_add(errors, moduleName, &conditionToken->cursor, 1, "Условие должно содержать выражение, возврщающее тип \"Буль\".");
		return false;
	}

	LLVMValueRef theFunction = LLVMGetBasicBlockParent(LLVMGetInsertBlock(llvmData->builder));
	KarString* thenString = kar_string_create_format("then%lu", llvmData->counter);
	LLVMBasicBlockRef thenBlock = LLVMAppendBasicBlock(theFunction, thenString);
	KAR_FREE(thenString);
	KarString* elseString = kar_string_create_format("else%lu", llvmData->counter);
	LLVMBasicBlockRef elseBlock = LLVMAppendBasicBlock(theFunction, elseString);
	KAR_FREE(elseString);
	KarString* mergeString = kar_string_create_format("merge%lu", llvmData->counter);
	LLVMBasicBlockRef mergeBlock = LLVMAppendBasicBlock(theFunction, mergeString);
	KAR_FREE(mergeString);
	llvmData->counter++;
	LLVMBuildCondBr(llvmData->builder, condition.value, thenBlock, elseBlock);

	LLVMPositionBuilderAtEnd(llvmData->builder, thenBlock);
	if (!generate_block_body(kar_token_child_get(token, i + 1), llvmData, moduleName, vars, errors)) {
		return false;
	}
	LLVMBuildBr(llvmData->builder, mergeBlock);
	thenBlock = LLVMGetInsertBlock(llvmData->builder);

	LLVMPositionBuilderAtEnd(llvmData->builder, elseBlock);
	if (i + 2 < kar_token_child_count(token)) {
		if (!generate_one_if(token, i + 2, llvmData, moduleName, vars, errors)) {
			return false;
		}
	}
	LLVMBuildBr(llvmData->builder, mergeBlock);
	elseBlock = LLVMGetInsertBlock(llvmData->builder);

	LLVMPositionBuilderAtEnd(llvmData->builder, mergeBlock);
	LLVMValueRef phi = LLVMBuildPhi(llvmData->builder, LLVMVoidType(), "ph");
	LLVMValueRef phi_res = LLVMConstInt(LLVMInt1Type(), 0, 0);
	LLVMValueRef phi_res2 = LLVMConstInt(LLVMInt1Type(), 0, 0);
	LLVMAddIncoming(phi, &phi_res, &thenBlock, 1);
	LLVMAddIncoming(phi, &phi_res2, &elseBlock, 1);

	return true;

}

static bool generate_if(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	size_t count = kar_token_child_count(token);
	if (count % 2 != 0) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутрення ошибка при парсинге блока \"если\". Количество потомков блока нечётное.");
		return false;
	}

	return generate_one_if(token, 0, llvmData, moduleName, vars, errors);
}

static bool generate_algorithm(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	switch (token->type) {
	case (KAR_TOKEN_COMMAND_EXPRESSION):
		return kar_generate_expression(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_COMMAND_DECLARATION):
		return generate_declaration(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_COMMAND_CONST_DECLARATION):
		return generate_const_declaration(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_COMMAND_ASSIGN):
		return generate_assign(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_COMMAND_BLOCK):
		return generate_block(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_COMMAND_CLEAN):
		return generate_clean(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_COMMAND_IF):
		return generate_if(token, llvmData, moduleName, vars, errors);
	default:
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Токен не является командой.");
		LLVMBuildRetVoid(llvmData->builder);
		return false;
	}
}

bool kar_generate_algorithm(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (!generate_algorithm(child, llvmData, moduleName, vars, errors)) {
			LLVMBuildRetVoid(llvmData->builder);
			return false;
		}
	}
	return true;
}


