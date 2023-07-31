/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/gen_expression.h"

#include <string.h>
#include <errno.h>

#include <llvm-c/Core.h>

LLVMValueRef puts_func;

/*static bool print(const KarString* out, KarLLVMData* llvmData) {
	if (puts_func == NULL) {
		LLVMTypeRef type = LLVMPointerType(LLVMInt8Type(), 0);
		LLVMTypeRef puts_type = LLVMFunctionType(LLVMVoidType(), &type, 1, false);
		puts_func = LLVMAddFunction(llvmData->module, "_kartarika_library_write_chars", puts_type);
	}

	LLVMValueRef out_string = LLVMBuildGlobalStringPtr(llvmData->builder, out, "helloWorld");
	LLVMBuildCall(llvmData->builder, puts_func, &out_string, 1, "_kartarika_library_write_chars");

	return true;
}*/

typedef struct {
	KarVartree* type;
	LLVMValueRef value;
} KarExpressionResult;

KarExpressionResult kar_expression_result_bad() {
	KarExpressionResult result;
	result.type = NULL;
	result.value = NULL;
	return result;
}

static bool kar_expression_result_is_good(KarExpressionResult result) {
	return (result.type != NULL || result.value != NULL);
}

static KarExpressionResult calc_expression(KarVartree *context, KarToken* token, KarLLVMData *llvmData, KarString *moduleName, KarVars *vars, KarProjectErrorList *errors);

static KarExpressionResult get_val_true(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.boolType;
	result.value = LLVMConstInt(LLVMInt1Type(), 1, 0);
	return result;
}

static KarExpressionResult get_val_false(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.boolType;
	result.value = LLVMConstInt(LLVMInt1Type(), 0, 0);
	return result;
}

static KarExpressionResult get_val_integer(KarToken* token, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	// TODO: Эту проверку необходимо перенести в анализатор.
	// TODO: Проверку надо более тщательно организовать. Сейчас работает только проверка на длину строки.
	if (strlen(token->str) > 11) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Слишком большое число.");
		return kar_expression_result_bad();
	}
	int32_t val;
	if (1 != sscanf(token->str, "%"SCNd32, &val)) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Не корректное число.");
		return kar_expression_result_bad();
	}
	KarExpressionResult result;
	result.type = vars->standard.int32Type;
	result.value = LLVMConstInt(LLVMInt32Type(), (long long unsigned int)val, 0);
	return result;
}

static KarExpressionResult get_val_hexadecimal(KarToken* token, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	// TODO: Эту проверку необходимо перенести в анализатор.
	if (strlen(token->str) > 7) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Слишком большое шестнадцатеричное число.");
		return kar_expression_result_bad();
	}
	uint32_t val;
	if (1 != sscanf(token->str + 3, "%x", &val)) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Не корректное шестнадцатеричное число.");
		return kar_expression_result_bad();
	}
	KarExpressionResult result;
	result.type = vars->standard.unsigned32Type;
	result.value = LLVMConstInt(LLVMInt32Type(), (long long unsigned int)val, 0);
	return result;
}

static KarExpressionResult get_val_float(KarToken* token, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	double d;
	// TODO: Доделать все особые случаи.
	if (kar_string_equal(token->str, "НеЧисло")) {
		d = NAN;
	} else {
		KarString* tmp1 = kar_string_create_replace(token->str, ",", ".");
		KarString* tmp2 = kar_string_create_replace(tmp1, "с", "E");
		KarString* tmp3 = kar_string_create_replace(tmp2, "С", "E");
		d = strtod(tmp3, NULL);
		KAR_FREE(tmp1);
		KAR_FREE(tmp2);
		KAR_FREE(tmp3);
		if (isnan(d) || isinf(d)) {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Не корректное дробное число.");
			return kar_expression_result_bad();
		}
	}

	if (errno == ERANGE) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Не корректное дробное число.");
		return kar_expression_result_bad();
	}

	KarExpressionResult result;
	result.type = vars->standard.float64Type;
	result.value = LLVMConstReal(LLVMDoubleType(), d);
	return result;
}

static KarExpressionResult get_val_nan(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.float64Type;
	result.value = LLVMConstReal(LLVMDoubleType(), 0.0 / 0.0);
	return result;
}

static KarExpressionResult get_val_infinity(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.float64Type;
	result.value = LLVMConstReal(LLVMDoubleType(), INFINITY);
	return result;
}

static KarExpressionResult get_val_minus_infinity(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.float64Type;
	result.value = LLVMConstReal(LLVMDoubleType(), -INFINITY);
	return result;
}

static KarExpressionResult get_val_string(KarToken* token, KarLLVMData* llvmData, KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.stringType;
	result.value = LLVMBuildGlobalStringPtr(llvmData->builder, token->str, "_kartarika_string");
	return result;
}

static KarExpressionResult get_field(KarVartree* context, KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* left = kar_token_child_get(token, 0);
	KarToken* right = kar_token_child_get(token, 1);
	KarVartree* newContext;
	if (context == NULL) {
		newContext = kar_vars_find(vars, left->str);
	} else {
		newContext = kar_vartree_find(context, left->str);
	}
	if (newContext == NULL) {
		KarString* error_text = kar_string_create_format("Не могу найти поле \"%s\" в объекте \"%s\".", left->str, kar_vartree_create_full_path(context));
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, error_text);
		KAR_FREE(error_text);
		return kar_expression_result_bad();
	}
	return calc_expression(newContext, right, llvmData, moduleName, vars, errors);
}

static KarExpressionResult get_call_method(KarVartree* context, KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KAR_CREATES(argsVartree, KarVartree*, kar_token_child_count(token));
	KAR_CREATES(argsLLVM, LLVMValueRef, kar_token_child_count(token));
	size_t num = 0;
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		KarToken* child = kar_token_child_get(token , i);
		if (child->type == KAR_TOKEN_SIGN_ARGUMENT) {
			KarExpressionResult res = calc_expression(context, kar_token_child_get(child, 0), llvmData, moduleName, vars, errors);
			if (!kar_expression_result_is_good(res)) {
				KAR_FREE(argsVartree);
				KAR_FREE(argsLLVM);
				return res;
			}
			argsVartree[num] = res.type;
			argsLLVM[num] = res.value;
			num++;
		}
	}

	KarToken* funcName = kar_token_child_get(token, 0);
	KarString* functionName = kar_vartree_create_full_function_name(funcName->str, argsVartree, num);
	KAR_FREE(argsVartree);

	KarVartree* function = kar_vartree_find(context, functionName);
	KAR_FREE(functionName);
	if (function == NULL) {
		kar_project_error_list_create_add(errors, moduleName, &funcName->cursor, 1, "Не могу найти объект \"Консоль.Вывод(Кар.Типы.Буль)\".");
		KAR_FREE(argsLLVM);
		return kar_expression_result_bad();
	}
	KarVartreeFunctionParams* params = kar_vartree_get_function_params(function);
	KarLLVMFunction* llvmFunc = kar_llvm_data_get_function(llvmData, params, vars);
	if (llvmFunc == NULL) {
		kar_project_error_list_create_add(errors, moduleName, &funcName->cursor, 1, "Внутренняя ошибка генератора. Не могу найти функцию.");
		KAR_FREE(argsLLVM);
		return kar_expression_result_bad();
	}

	KarExpressionResult result;
	result.type = params->returnType;
	result.value = LLVMBuildCall(llvmData->builder, kar_llvm_function_get_ref(llvmFunc), argsLLVM, (unsigned int)num, llvmFunc->params->issueName);
	KAR_FREE(argsLLVM);
	return result;
}

static KarExpressionResult calc_expression(KarVartree* context, KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	switch (token->type) {
	case (KAR_TOKEN_VAL_TRUE): return get_val_true(vars);
	case (KAR_TOKEN_VAL_FALSE): return get_val_false(vars);
	case (KAR_TOKEN_VAL_INTEGER): return get_val_integer(token, moduleName, vars, errors);
	case (KAR_TOKEN_VAL_HEXADECIMAL): return get_val_hexadecimal(token, moduleName, vars, errors);
	case (KAR_TOKEN_VAL_FLOAT): return get_val_float(token, moduleName, vars, errors);
	case (KAR_TOKEN_VAL_NAN): return get_val_nan(vars);
	case (KAR_TOKEN_VAL_INFINITY): return get_val_infinity(vars);
	case (KAR_TOKEN_VAL_MINUS_INFINITY): return get_val_minus_infinity(vars);
	case (KAR_TOKEN_VAL_STRING): return get_val_string(token, llvmData, vars);

	case(KAR_TOKEN_SIGN_GET_FIELD): return get_field(context, token, llvmData, moduleName, vars, errors);
	case(KAR_TOKEN_SIGN_CALL_METHOD): return get_call_method(context, token, llvmData, moduleName, vars, errors);
	default: return kar_expression_result_bad();
	}
}

bool kar_generate_expression(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_COMMAND_EXPRESSION) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Токен не является командой.");
		return false;
	}
	if (kar_token_child_count(token) != 1) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутренняя ошибка. Количество потомков выражения в дереве тое=кенов не равно 1.");
		return false;
	}
	KarToken* child = kar_token_child_get(token, 0);
	return kar_expression_result_is_good(calc_expression(NULL, child, llvmData, moduleName, vars, errors));
}

