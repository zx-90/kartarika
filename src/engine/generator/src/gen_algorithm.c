/* Copyright © 2024 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/gen_algorithm.h"

#include "generator/gen_expression.h"

#include <llvm-c/Core.h>

static bool generate_algorithm(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors);

/*static bool generate_expression(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	kar_generate_expression(token, llvmData, moduleName, vars, errors);
}*/

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

static bool generate_block(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	if (kar_token_child_count(token) != 1) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутренняя ошибка. Количество потомков блока должно быть равно 1.");
		return false;
	}
	KarToken* block_body = kar_token_child_get(token, 0);
	if (block_body->type != KAR_TOKEN_BLOCK_BODY) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутренняя ошибка. Тип потомка блока не является его телом.");
		return false;
	}
	kar_local_stack_block_insert(vars->locals, kar_local_block_create(), 0);
	if (!kar_generate_algorithm(block_body, llvmData, moduleName, vars, errors)) {
		return false;
	}
	kar_local_stack_block_erase(vars->locals, 0);
	return true;
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

		/*if (child->type == KAR_TOKEN_COMMAND_EXPRESSION) {
			if (!kar_generate_expression(child, llvmData, moduleName, vars, errors)) {
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
		} else if (child->type == KAR_TOKEN_COMMAND_DECLARATION) {
			KarToken* varNameToken = kar_token_child_get(child, 0);
			if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
				kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Правая часть объявления переменной имеет не корректное имя.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarLocalBlock* block = kar_local_stack_block_get(vars->locals, 0);
			if (kar_local_block_get_var_by_name(block, varNameToken->str) != NULL) {
				kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Переменная с таким именем уже существует.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarToken* expressionToken = kar_token_child_get(child, 1);
			KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, moduleName, vars, errors);
			if (kar_expression_result_is_none(result)) {
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarLocalVar* var = kar_local_var_create(varNameToken->str, result.type, result.value);
			kar_local_block_var_add(block, var);
		} else if (child->type == KAR_TOKEN_COMMAND_CONST_DECLARATION) {
			KarToken* varNameToken = kar_token_child_get(child, 0);
			if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
				kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Правая часть объявления переменной имеет не корректное имя.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarLocalBlock* block = kar_local_stack_block_get(vars->locals, 0);
			if (kar_local_block_get_var_by_name(block, varNameToken->str) != NULL) {
				kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Переменная с таким именем уже существует.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarToken* expressionToken = kar_token_child_get(child, 1);
			KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, moduleName, vars, errors);
			if (kar_expression_result_is_none(result)) {
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarLocalVar* var = kar_local_var_create_const(varNameToken->str, result.type, result.value);
			kar_local_block_var_add(block, var);
		} else if (child->type == KAR_TOKEN_COMMAND_ASSIGN) {
			KarToken* varNameToken = kar_token_child_get(child, 0);
			if (varNameToken->type != KAR_TOKEN_IDENTIFIER) {
				kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Правая часть  имеет не корректное имя.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarLocalBlock* block = kar_local_stack_block_get(vars->locals, 0);
			KarLocalVar* var = kar_local_block_get_var_by_name(block, varNameToken->str);
			if (var == NULL) {
				kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Переменной с таким именем не существует.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			if (var->is_const) {
				kar_project_error_list_create_add(errors, moduleName, &varNameToken->cursor, 1, "Попытка присвоения константе нового значения.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarToken* expressionToken = kar_token_child_get(child, 1);
			KarExpressionResult result = kar_generate_calc_expression(expressionToken, llvmData, moduleName, vars, errors);
			if (kar_expression_result_is_none(result)) {
				LLVMBuildRetVoid(llvmData->builder);
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
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			var->value = result.value;
		} else if (child->type == KAR_TOKEN_COMMAND_BLOCK) {
			if (kar_token_child_count(child) != 1) {
				kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Внутренняя ошибка. Количество потомков блока должно быть равно 1.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			KarToken* block_body = kar_token_child_get(child, 0);
			if (block_body->type != KAR_TOKEN_BLOCK_BODY) {
				kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Внутренняя ошибка. Тип потомка блока не является его телом.");
				LLVMBuildRetVoid(llvmData->builder);
				return false;
			}
			kar_local_stack_block_insert(vars->locals, kar_local_block_create(), 0);
			if (!kar_generate_algorithm(block_body, llvmData, moduleName, vars, errors)) {
				return false;
			}
			kar_local_stack_block_erase(vars->locals, 0);
		} else {
			kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Токен не является командой.");
			LLVMBuildRetVoid(llvmData->builder);
			return false;
		}*/
	}
	return true;
}


