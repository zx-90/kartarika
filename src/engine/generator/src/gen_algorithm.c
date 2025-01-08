/* Copyright © 2024 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/gen_algorithm.h"

#include <llvm-c/Core.h>

#include "model/vartree_class.h"
#include "model/vartree_var.h"

#include "generator/gen_expression.h"

typedef struct {
	LLVMBasicBlockRef continueLabel;
	LLVMBasicBlockRef breakLabel;
} KarBlockParams;

/*
Эти две функции написаны, чтобы обойти багу LLVM.
Так как если пишешь 2 перехода подряд:

	br label %exit0
	br label %merge1

То он переходит по последней ссылке. А в откомпилированном *.ll файле
переходит по первой метке. Так что приходится убирать вторую и дальше метки.

*/

static void set_jump(LLVMBasicBlockRef ref, KarLLVMData* llvmData, KarVars* vars) {
	if (!vars->locals->params) {
		LLVMBuildBr(llvmData->builder, ref);
	}
	vars->locals->params = (void*)true;
}

static LLVMBasicBlockRef end_block(KarLLVMData* llvmData, KarVars* vars) {
	vars->locals->params = false;
	return LLVMGetInsertBlock(llvmData->builder);
}

static bool generate_algorithm(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors);

static bool generate_declaration(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* varNameToken = kar_token_child_get(token, 0);
	if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
		kar_project_error_list_create_add(errors, module->name, &varNameToken->cursor, 1, "Правая часть объявления переменной имеет не корректное имя.");
		return false;
	}
	KarLocalBlock* block = kar_local_stack_block_get(vars->locals, 0);
	if (kar_local_block_get_var_by_name(block, varNameToken->str) != NULL) {
		kar_project_error_list_create_add(errors, module->name, &varNameToken->cursor, 1, "Переменная с таким именем уже существует.");
		return false;
	}
	KarToken* expressionToken = kar_token_child_get(token, 1);
	KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, module, vars, errors);
	if (kar_expression_result_is_none(result)) {
		return false;
	}
	LLVMTypeRef type = kar_expression_get_type_by_vartype(vars, result.type);
	if (type == NULL) {
		kar_project_error_list_create_add(errors, module->name, &expressionToken->cursor, 1, "Неизвестный тип для присваивания переменной.");
		return false;
	}
	LLVMValueRef varAlloc = LLVMBuildAlloca(llvmData->builder, type, varNameToken->str);
	LLVMBuildStore(llvmData->builder, kar_expression_get_reduced_value(result.type, result.value, llvmData, vars), varAlloc);
	KarLocalVar* var = kar_local_var_create(
		varNameToken->str,
		kar_expression_get_reduced_type(result.type, vars),
		varAlloc
	);
	kar_local_block_var_add(block, var);
	return true;
}

static bool generate_const_declaration(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* varNameToken = kar_token_child_get(token, 0);
	if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
		kar_project_error_list_create_add(errors, module->name, &varNameToken->cursor, 1, "Правая часть объявления переменной имеет не корректное имя.");
		return false;
	}
	KarLocalBlock* block = kar_local_stack_block_get(vars->locals, 0);
	if (kar_local_block_get_var_by_name(block, varNameToken->str) != NULL) {
		kar_project_error_list_create_add(errors, module->name, &varNameToken->cursor, 1, "Переменная с таким именем уже существует.");
		return false;
	}
	KarToken* expressionToken = kar_token_child_get(token, 1);
	KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, module, vars, errors);
	if (kar_expression_result_is_none(result)) {
		return false;
	}
	KarLocalVar* var = kar_local_var_create_const(varNameToken->str, result.type, result.value);
	kar_local_block_var_add(block, var);
	return true;
}

static bool generate_assign(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* varNameToken = kar_token_child_get(token, 0);
	if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
		kar_project_error_list_create_add(errors, module->name, &varNameToken->cursor, 1, "Правая часть  имеет не корректное имя.");
		return false;
	}
	KarLocalVar* var = kar_vars_local_find(vars, varNameToken->str);
	if (var == NULL) {
		KarVartree* rootVar = kar_vartree_find(module, varNameToken->str);
		if (rootVar != NULL) {
			if (rootVar->type != KAR_VARTYPE_VARIABLE) {
				kar_project_error_list_create_add(errors, module->name, &varNameToken->cursor, 1, "Присваивание можно производит только для переменной.");
				return false;
			}
			KarToken* expressionToken = kar_token_child_get(token, 1);
			KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, module, vars, errors);
			if (kar_expression_result_is_none(result)) {
				return false;
			}
			KarVartreeConstValue* params = (KarVartreeConstValue*)rootVar->params;
			if (result.type != params->type) {
				kar_project_error_list_create_add(errors, module->name, &expressionToken->cursor, 1, "Тип переменной и тип выражения не совпадают.");
				return false;
			}
			LLVMBuildStore(llvmData->builder, kar_expression_get_reduced_value(result.type, result.value, llvmData, vars), (LLVMValueRef)rootVar->generatorParams);
			return true;
		} else {
			kar_project_error_list_create_add(errors, module->name, &varNameToken->cursor, 1, "Переменной с таким именем не существует.");
			return false;
		}
	} else {
		if (var->is_const) {
			kar_project_error_list_create_add(errors, module->name, &varNameToken->cursor, 1, "Попытка присвоения константе нового значения.");
			return false;
		}
		KarToken* expressionToken = kar_token_child_get(token, 1);
		KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, module, vars, errors);
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
			kar_project_error_list_create_add(errors, module->name, &expressionToken->cursor, 1, errorStr);
			KAR_FREE(errorStr);
			return false;
		}
		LLVMBuildStore(llvmData->builder, result.value, var->value);
	}
	return true;
}

static bool generate_block_body(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_BLOCK_BODY) {
		kar_project_error_list_create_add(errors, module->name, &token->cursor, 1, "Внутренняя ошибка. Тип потомка блока не является его телом.");
		return false;
	}
	kar_local_stack_block_insert(vars->locals, kar_local_block_create(), 0);
	if (!kar_generate_algorithm(token, llvmData, module, vars, errors)) {
		kar_local_stack_block_erase(vars->locals, 0);
		return false;
	}
	kar_local_stack_block_erase(vars->locals, 0);
	return true;
}

static bool generate_block(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	if (kar_token_child_count(token) != 1) {
		kar_project_error_list_create_add(errors, module->name, &token->cursor, 1, "Внутренняя ошибка. Количество потомков блока должно быть равно 1.");
		return false;
	}
	KarToken* block_body = kar_token_child_get(token, 0);
	return (generate_block_body(block_body, llvmData, module, vars, errors));
}

static bool generate_clean(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* uncleanedToken = kar_token_child_get(token, 0);
	KarExpressionResult uncleaned = kar_generate_calc_expression(uncleanedToken, llvmData, module, vars, errors);
	if (kar_expression_result_is_none(uncleaned)) {
		return false;
	}
	if (uncleaned.type->type != KAR_VARTYPE_UNCLEAN_CLASS) {
		kar_project_error_list_create_add(errors, module->name, &uncleanedToken->cursor, 1, "Переменная не является неопределённостью.");
		return false;
	}

	KarVartree* function = kar_vars_find_child(vars, vars->standard.unclean, "ПустойЛи", NULL, 0);
	KarLLVMFunction* llvmFunc = kar_llvm_data_get_function(llvmData, function, vars);
	LLVMValueRef expressionValue = LLVMBuildCall(llvmData->builder, kar_llvm_function_get_ref(llvmFunc), (LLVMValueRef*)&uncleaned.value, 1, "var");

	LLVMValueRef theFunction = LLVMGetBasicBlockParent(end_block(llvmData, vars));
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
		if (!generate_block_body(elseToken, llvmData, module, vars, errors)) {
			return false;
		}
	}

	set_jump(mergeBlock, llvmData, vars);
	thenBlock = end_block(llvmData, vars);

	LLVMPositionBuilderAtEnd(llvmData->builder, elseBlock);

	KarToken* thenToken = kar_token_child_get(token, 2);
	if (thenToken->type != KAR_TOKEN_BLOCK_BODY) {
		kar_project_error_list_create_add(errors, module->name, &token->cursor, 1, "Внутренняя ошибка. Тип потомка блока не является его телом.");
		return false;
	}

	KarString* cleanName;
	KarToken* tokenName = kar_token_child_get(token, 1);
	if (tokenName->type == KAR_TOKEN_VAL_NULL) {
		if (uncleanedToken->type == KAR_TOKEN_IDENTIFIER) {
			cleanName = uncleanedToken->str;
		} else {
			kar_project_error_list_create_add(errors, module->name, &tokenName->cursor, 1, "Раскрываемое выражение должно иметь идентификатор после ключевого слова \"как\".");
			return false;
		}
	} else if (tokenName->type == KAR_TOKEN_IDENTIFIER) {
		cleanName = tokenName->str;
	} else {
		kar_project_error_list_create_add(errors, module->name, &tokenName->cursor, 1, "Выражение после ключевого слова \"как\" должно быть идентификатором.");
		return false;
	}
	KarVartree* cleanType = kar_vartree_args_get(uncleaned.type, 0);
	LLVMValueRef cleanValue = LLVMBuildCall(llvmData->builder, kar_llvm_data_get_clean_function_by_type(llvmData, cleanType), (LLVMValueRef*)&uncleaned.value, 1, "");

	kar_local_stack_block_insert(vars->locals, kar_local_block_create(), 0);
	KarLocalBlock* subblock = kar_local_stack_block_get(vars->locals, 0);
	// TODO: При раскрытии переменная становится константой, её нельзя менять внутри блока.
	// Надо бы исправить на переменную.
	KarLocalVar* var = kar_local_var_create_const(cleanName, cleanType, cleanValue);
	kar_local_block_var_add(subblock, var);

	if (!kar_generate_algorithm(thenToken, llvmData, module, vars, errors)) {
		kar_local_stack_block_erase(vars->locals, 0);
		return false;
	}
	kar_local_stack_block_erase(vars->locals, 0);

	set_jump(mergeBlock, llvmData, vars);
	elseBlock = end_block(llvmData, vars);

	LLVMPositionBuilderAtEnd(llvmData->builder, mergeBlock);
	LLVMValueRef phi = LLVMBuildPhi(llvmData->builder, LLVMInt1Type(), "ph");
	LLVMValueRef phi_res = LLVMConstInt(LLVMInt1Type(), 0, 0);
	LLVMValueRef phi_res2 = LLVMConstInt(LLVMInt1Type(), 0, 0);
	LLVMAddIncoming(phi, &phi_res, &thenBlock, 1);
	LLVMAddIncoming(phi, &phi_res2, &elseBlock, 1);

	return true;
}

static bool generate_one_if(KarToken* token, size_t i, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* conditionToken = kar_token_child_get(token, i);
	KarExpressionResult condition = kar_generate_calc_expression(conditionToken, llvmData, module, vars, errors);
	if (kar_expression_result_is_none(condition)) {
		return false;
	}
	if (kar_vartree_get_class_type(condition.type) != KAR_CLASS_TYPE_BOOL) {
		kar_project_error_list_create_add(errors, module->name, &conditionToken->cursor, 1, "Условие должно содержать выражение, возврщающее тип \"Буль\".");
		return false;
	}

	LLVMValueRef theFunction = LLVMGetBasicBlockParent(end_block(llvmData, vars));
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
	if (!generate_block_body(kar_token_child_get(token, i + 1), llvmData, module, vars, errors)) {
		return false;
	}
	set_jump(mergeBlock, llvmData, vars);
	thenBlock = end_block(llvmData, vars);

	LLVMPositionBuilderAtEnd(llvmData->builder, elseBlock);
	if (i + 2 < kar_token_child_count(token)) {
		if (!generate_one_if(token, i + 2, llvmData, module, vars, errors)) {
			return false;
		}
	}
	set_jump(mergeBlock, llvmData, vars);
	elseBlock = end_block(llvmData, vars);

	LLVMPositionBuilderAtEnd(llvmData->builder, mergeBlock);

	return true;
}

static bool generate_if(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	size_t count = kar_token_child_count(token);
	if (count % 2 != 0) {
		kar_project_error_list_create_add(errors, module->name, &token->cursor, 1, "Внутрення ошибка при парсинге блока \"если\". Количество потомков блока нечётное.");
		return false;
	}

	return generate_one_if(token, 0, llvmData, module, vars, errors);
}

static bool generate_while(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	LLVMValueRef theFunction = LLVMGetBasicBlockParent(end_block(llvmData, vars));
	KarString* headerString = kar_string_create_format("header%lu", llvmData->counter);
	LLVMBasicBlockRef headerBlock = LLVMAppendBasicBlock(theFunction, headerString);
	KAR_FREE(headerString);
	KarString* bodyString = kar_string_create_format("body%lu", llvmData->counter);
	LLVMBasicBlockRef bodyBlock = LLVMAppendBasicBlock(theFunction, bodyString);
	KAR_FREE(bodyString);
	KarString* exitString = kar_string_create_format("exit%lu", llvmData->counter);
	LLVMBasicBlockRef exitBlock = LLVMAppendBasicBlock(theFunction, exitString);
	KAR_FREE(exitString);
	llvmData->counter++;
	set_jump(headerBlock, llvmData, vars);

	LLVMPositionBuilderAtEnd(llvmData->builder, headerBlock);
	KarToken* conditionToken = kar_token_child_get(token, 0);
	KarExpressionResult condition = kar_generate_calc_expression(conditionToken, llvmData, module, vars, errors);
	if (kar_expression_result_is_none(condition)) {
		return false;
	}
	if (kar_vartree_get_class_type(condition.type) != KAR_CLASS_TYPE_BOOL) {
		kar_project_error_list_create_add(errors, module->name, &conditionToken->cursor, 1, "Условие должно содержать выражение, возврщающее тип \"Буль\".");
		return false;
	}
	LLVMBuildCondBr(llvmData->builder, condition.value, bodyBlock, exitBlock);
	headerBlock = end_block(llvmData, vars);

	LLVMPositionBuilderAtEnd(llvmData->builder, bodyBlock);
	KarToken* blockBody = kar_token_child_get(token, 1);
	if (blockBody->type != KAR_TOKEN_BLOCK_BODY) {
		kar_project_error_list_create_add(errors, module->name, &blockBody->cursor, 1, "Внутренняя ошибка. Тип потомка блока не является его телом.");
		return false;
	}
	KarLocalBlock* localBlock = kar_local_block_create();
	KarBlockParams params = {headerBlock, exitBlock};
	localBlock->blockParams = &params;
	kar_local_stack_block_insert(vars->locals, localBlock, 0);

	if (!kar_generate_algorithm(blockBody, llvmData, module, vars, errors)) {
		kar_local_stack_block_erase(vars->locals, 0);
		return false;
	}
	kar_local_stack_block_erase(vars->locals, 0);

	set_jump(headerBlock, llvmData, vars);
	bodyBlock = end_block(llvmData, vars);

	LLVMPositionBuilderAtEnd(llvmData->builder, exitBlock);

	return true;
}

static bool generate_break(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	KarLocalStack* stack = vars->locals;
	size_t count = 0;
	for (size_t i = 0; i < kar_local_stack_block_count(stack); i++) {
		KarLocalBlock* block = kar_local_stack_block_get(stack, i);
		if (block->blockParams == NULL) {
			continue;
		}
		if (count + 1 == kar_token_child_count(token)) {
			KarToken* last = kar_token_child_get(token, count);
			if (last->type == KAR_TOKEN_COMMAND_BREAK) {
				set_jump(((KarBlockParams*)block->blockParams)->breakLabel, llvmData, vars);
				return true;
			} else if (last->type == KAR_TOKEN_COMMAND_CONTINUE) {
				set_jump(((KarBlockParams*)block->blockParams)->continueLabel, llvmData, vars);
				return true;
			} else {
				kar_project_error_list_create_add(errors, module->name, &last->cursor, 1, "Неизвестная команда перехода.");
				return false;
			}
		}
		count++;
	}
	kar_project_error_list_create_add(errors, module->name, &kar_token_child_get(token, count)->cursor, 1, "Слишком длинная команда перехода.");
	return false;
}

static bool generate_algorithm(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	switch (token->type) {
	case (KAR_TOKEN_COMMAND_EXPRESSION):
		return kar_generate_expression(token, llvmData, module, vars, errors);
	case (KAR_TOKEN_COMMAND_DECLARATION):
		return generate_declaration(token, llvmData, module, vars, errors);
	case (KAR_TOKEN_COMMAND_CONST_DECLARATION):
		return generate_const_declaration(token, llvmData, module, vars, errors);
	case (KAR_TOKEN_COMMAND_ASSIGN):
		return generate_assign(token, llvmData, module, vars, errors);
	case (KAR_TOKEN_COMMAND_BLOCK):
		return generate_block(token, llvmData, module, vars, errors);
	case (KAR_TOKEN_COMMAND_CLEAN):
		return generate_clean(token, llvmData, module, vars, errors);
	case (KAR_TOKEN_COMMAND_IF):
		return generate_if(token, llvmData, module, vars, errors);
	case (KAR_TOKEN_COMMAND_WHILE):
		return generate_while(token, llvmData, module, vars, errors);
	case (KAR_TOKEN_COMMAND_BREAK):
		return generate_break(token, llvmData, module, vars, errors);
	default:
		kar_project_error_list_create_add(errors, module->name, &token->cursor, 1, "Токен не является командой.");
		LLVMBuildRetVoid(llvmData->builder);
		return false;
	}
}

bool kar_generate_algorithm(KarToken* token, KarLLVMData* llvmData, KarVartree* module, KarVars* vars, KarProjectErrorList* errors) {
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (!generate_algorithm(child, llvmData, module, vars, errors)) {
			LLVMBuildRetVoid(llvmData->builder);
			return false;
		}
	}
	return true;
}
