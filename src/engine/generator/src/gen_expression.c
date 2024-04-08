/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/gen_expression.h"

#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#include "core/unicode.h"

#include <llvm-c/Core.h>

// TODO: Перенести в core. Создать отдельный файл для математики.
#define DNAN (INFINITY * 0.0)

typedef struct {
	KarVartree* type;
	LLVMValueRef value;
} KarExpressionResult;

KarExpressionResult kar_expression_result_none() {
	KarExpressionResult result;
	result.type = NULL;
	result.value = NULL;
	return result;
}

KarExpressionResult kar_expression_result_var(KarVartree* var) {
	KarExpressionResult result;
	result.type = var;
	result.value = NULL;
	return result;
}

static bool kar_expression_result_is_none(KarExpressionResult result) {
	return (result.type == NULL && result.value == NULL);
}

static bool cast_type(KarExpressionResult* varFrom, KarExpressionResult* varTo, KarLLVMData *llvmData, KarVars *vars) {
	if (
		varFrom->type == vars->standard.decimalType
	) {
		if (varTo->type == vars->standard.int8Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt8Type(), "decimal_to_int8");
			return true;
		} else if (varTo->type == vars->standard.int16Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt16Type(), "decimal_to_int16");
			return true;
		} else if (varTo->type == vars->standard.int32Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt32Type(), "decimal_to_int32");
			return true;
		} else if (varTo->type == vars->standard.int64Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt64Type(), "decimal_to_int64");
			return true;
		} else if (varTo->type == vars->standard.unsigned8Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt8Type(), "decimal_to_unsigned8");
			return true;
		} else if (varTo->type == vars->standard.unsigned16Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt16Type(), "decimal_to_unsigned16");
			return true;
		} else if (varTo->type == vars->standard.unsigned32Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt32Type(), "decimal_to_unsigned32");
			return true;
		} else if (varTo->type == vars->standard.unsigned64Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt64Type(), "decimal_to_unsigned64");
			return true;
		} else if (varTo->type == vars->standard.float32Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMSIToFP, varFrom->value, LLVMInt64Type(), "decimal_to_float32");
			return true;
		} else if (varTo->type == vars->standard.float64Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMSIToFP, varFrom->value, LLVMInt64Type(), "decimal_to_float64");
			return true;
		}
	} else if (varFrom->type == vars->standard.hexadecimalType) {
		if (varTo->type == vars->standard.int8Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt8Type(), "hex_to_int8");
			return true;
		} else if (varTo->type == vars->standard.int16Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt16Type(), "hex_to_int16");
			return true;
		} else if (varTo->type == vars->standard.int32Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt32Type(), "hex_to_int32");
			return true;
		} else if (varTo->type == vars->standard.int64Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt64Type(), "hex_to_int64");
			return true;
		} else if (varTo->type == vars->standard.unsigned8Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt8Type(), "hex_to_unsigned8");
			return true;
		} else if (varTo->type == vars->standard.unsigned16Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt16Type(), "hex_to_unsigned16");
			return true;
		} else if (varTo->type == vars->standard.unsigned32Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt32Type(), "hex_to_unsigned32");
			return true;
		} else if (varTo->type == vars->standard.unsigned64Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMTrunc, varFrom->value, LLVMInt64Type(), "hex_to_unsigned64");
			return true;
		} else if (varTo->type == vars->standard.float32Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMUIToFP, varFrom->value, LLVMInt64Type(), "hex_to_float32");
			return true;
		} else if (varTo->type == vars->standard.float64Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMUIToFP, varFrom->value, LLVMInt64Type(), "hex_to_float64");
			return true;
		}
	} else if (varFrom->type == vars->standard.literalFloat) {
		if (varTo->type == vars->standard.float32Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMFPTrunc, varFrom->value, LLVMFloatType(), "literal_double_to_float");
			return true;
		} else if (varTo->type == vars->standard.float64Type) {
			varFrom->type = varTo->type;
			varFrom->value = LLVMBuildCast(llvmData->builder, LLVMFPTrunc, varFrom->value, LLVMDoubleType(), "literal_double_to_double");
			return true;
		}
	}
	return false;
}

static bool check_and_cast_types(KarExpressionResult* var1, KarExpressionResult* var2, KarLLVMData *llvmData, KarVars *vars) {
	if (var1->type == var2->type) {
		return true;
	}
	return cast_type(var1, var2, llvmData, vars) ||
			cast_type(var2, var1, llvmData, vars);
}

static KarExpressionResult calc_expression(KarToken* token, KarLLVMData *llvmData, KarString *moduleName, KarVars *vars, KarProjectErrorList *errors);

static KarExpressionResult  get_val_null(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.nullType;
	result.value = LLVMConstNull(LLVMInt8Type());
	return result;
}

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
	KarString* end;
	errno = 0;
	long long unsigned int val = (long long unsigned int)strtoll(token->str, &end, 10);
	if (end < token->str + strlen(token->str)) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Не корректное число.");
		return kar_expression_result_none();
	}
	if ((errno == ERANGE && val == LLONG_MAX) || val > UINT64_MAX) {
		errno = 0;
		val = strtoull(token->str, &end, 10);
		if (errno == ERANGE || val > UINT64_MAX) {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Слишком большое число.");
			return kar_expression_result_none();
		}
	} else if ((errno == ERANGE && (long long int)val == LLONG_MIN) || (long long int)val < (long long int)INT64_MIN) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Слишком маленькое число.");
		return kar_expression_result_none();
	}

	KarExpressionResult result;
	result.type = vars->standard.decimalType;
	result.value = LLVMConstInt(LLVMInt64Type(), val, 0);
	return result;
}

static KarExpressionResult get_val_hexadecimal(KarToken* token, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	// TODO: Эту проверку необходимо перенести в анализатор.
	size_t len = strlen(token->str);
	if (len < 4) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутренняя ошибка. Длина идентификатора шестнадцатеричного числа менее 2 символов.");
		return kar_expression_result_none();
	}

	long long unsigned int val = 0;
	size_t unicodeLen = 0;
	bool zeroLed = false;
	uint32_t buffer = 0;
	for (size_t count = 3; count < len;) {
		val *= 16;
		buffer = kar_unicode_get(&token->str[count], &count);
		if (buffer >= 0x0030 && buffer <= 0x0039) {
			val += (buffer - 0x0030);
		} else if (buffer >= 0x0410 && buffer <= 0x0415) {
			val += ((long long unsigned int)buffer - 0x0410 + 10);
		} else if (buffer >= 0x0430 && buffer <= 0x0435) {
			val += ((long long unsigned int)buffer - 0x0430 + 10);
		} else {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Некорретная запись шестандцатеричного числа.");
			return kar_expression_result_none();
		}
		if (val != 0) {
			zeroLed = true;
		}
		if (zeroLed) {
			unicodeLen++;
			if (unicodeLen > 16) {
				kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Слишком длинное шестнадцатеричное число.");
				return kar_expression_result_none();
			}
		}
	}

	KarExpressionResult result;
	result.type = vars->standard.hexadecimalType;
	result.value = LLVMConstInt(LLVMInt64Type(), (long long unsigned int)val, 0);
	return result;
}

static KarExpressionResult get_val_float(KarToken* token, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	// TODO: Эту проверку необходимо перенести в анализатор.
	// TODO: Нет значений +/- бесконечность. Надо добавить.
	double d;
	errno = 0;
	if (kar_string_equal(token->str, "НеЧисло")) {
		d = DNAN;
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
			return kar_expression_result_none();
		}
	}

	if (errno == ERANGE) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Не корректное дробное число.");
		return kar_expression_result_none();
	}

	KarExpressionResult result;
	result.type = vars->standard.literalFloat;
	result.value = LLVMConstReal(LLVMDoubleType(), d);
	return result;
}

static KarExpressionResult get_val_nan(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.literalFloat;
	result.value = LLVMConstReal(LLVMDoubleType(), DNAN);
	return result;
}

static KarExpressionResult get_val_infinity(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.literalFloat;
	result.value = LLVMConstReal(LLVMDoubleType(), INFINITY);
	return result;
}

static KarExpressionResult get_val_minus_infinity(KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.literalFloat;
	result.value = LLVMConstReal(LLVMDoubleType(), -INFINITY);
	return result;
}

static KarExpressionResult get_val_char(KarString*str, KarLLVMData* llvmData, KarVars* vars) {
	KarExpressionResult result;
	result.type = vars->standard.stringType;

	LLVMValueRef ref = LLVMBuildGlobalStringPtr(llvmData->builder, str, "_kartarika_string");
	result.value = LLVMBuildCall(llvmData->builder, llvmData->createString, &ref, 1, "var");
	return result;
}

static KarVartree* get_new_context(KarVartree* context, KarString* name, KarVartree** args, size_t args_count, KarVars* vars) {
	if (context == NULL) {
		return kar_vars_find_args(vars, name, args, args_count);
	} else {
		return kar_vartree_find_args(context, name, args, args_count);
	}
}

static KarExpressionResult get_identifier(KarToken* token, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarExpressionResult res = kar_expression_result_none();
	res.type = kar_vars_find(vars, token->str);
	if (kar_expression_result_is_none(res)) {
		KarString* error_text = kar_string_create_format("Не могу найти поле \"%s\".", token->str);
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, error_text);
		KAR_FREE(error_text);
		return kar_expression_result_none();
	}
	return res;
}

static KarVartree* get_reduced_type(KarVartree* type, KarVars* vars) {
	if (type == vars->standard.decimalType) {
		return vars->standard.int32Type;
	}
	if (type == vars->standard.hexadecimalType) {
		return vars->standard.unsigned32Type;
	}
	if (type == vars->standard.literalFloat) {
		return vars->standard.float64Type;
	}
	return type;
}

static KarVartree* get_reduced64_type(KarVartree* type, KarVars* vars) {
	if (type == vars->standard.decimalType) {
		return vars->standard.int64Type;
	}
	if (type == vars->standard.hexadecimalType) {
		return vars->standard.unsigned64Type;
	}
	if (type == vars->standard.literalFloat) {
		return vars->standard.float64Type;
	}
	return type;
}

static KarExpressionResult get_open_braces(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	return calc_expression(kar_token_child_get(token, 0), llvmData, moduleName, vars, errors);
}

static KarExpressionResult get_field(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* left = kar_token_child_get(token, 0);
	KarToken* right = kar_token_child_get(token, 1);

	KarExpressionResult leftRes = calc_expression(left, llvmData, moduleName, vars, errors);
	if (leftRes.value != NULL) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Левый операнд должен быть классом, а не экземпляром класса.");
		return kar_expression_result_none();
	}
	if (kar_expression_result_is_none(leftRes)) {
		return kar_expression_result_none();
	}

	KarExpressionResult res = kar_expression_result_none();
	res.type = kar_vartree_find(leftRes.type, right->str);
	if (kar_expression_result_is_none(leftRes)) {
		KarString* error_text = kar_string_create_format("Не могу найти поле \"%s\" в объекте \"%s\".", right->str, kar_vartree_create_full_path(leftRes.type));
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, error_text);
		KAR_FREE(error_text);
		return kar_expression_result_none();
	}

	return res;
}

// TODO: Перенсти в нужное место, возможно есть копия.
static KarString* get_token_string(KarToken* token) {
	// TODO: Использовать макросы из лексера.
	if (token->type == KAR_TOKEN_VAR_BOOL) {
		return "Буль";
	} else if (token->type == KAR_TOKEN_VAR_INTEGER8) {
		return "Целое8";
	} else if (token->type == KAR_TOKEN_VAR_INTEGER16) {
		return "Целое16";
	} else if (token->type == KAR_TOKEN_VAR_INTEGER32) {
		return "Целое32";
	} else if (token->type == KAR_TOKEN_VAR_INTEGER64) {
		return "Целое64";
	} else if (token->type == KAR_TOKEN_VAR_UNSIGNED8) {
		return "Счётное8";
	} else if (token->type == KAR_TOKEN_VAR_UNSIGNED16) {
		return "Счётное16";
	} else if (token->type == KAR_TOKEN_VAR_UNSIGNED32) {
		return "Счётное32";
	} else if (token->type == KAR_TOKEN_VAR_UNSIGNED64) {
		return "Счётное64";
	} else if (token->type == KAR_TOKEN_VAR_FLOAT32) {
		return "Дробное32";
	} else if (token->type == KAR_TOKEN_VAR_FLOAT64) {
		return "Дробное64";
	} else if (token->type == KAR_TOKEN_VAR_STRING) {
		return "Строка";
	}
	return token->str;
}

static KarExpressionResult get_call_method(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarExpressionResult cont = kar_expression_result_none();
	KarString* funcName;

	KarToken* left = kar_token_child_get(token, 0);
	if (left->type == KAR_TOKEN_IDENTIFIER) {
		funcName = left->str;
	} else if (left->type == KAR_TOKEN_SIGN_GET_FIELD) {
		KarToken* contToken = kar_token_child_get(left, 0);
		KarToken* nameToken = kar_token_child_get(left, 1);
		if (nameToken->type != KAR_TOKEN_IDENTIFIER) {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Левая часть вызова функции не определена.");
			return kar_expression_result_none();
		}
		funcName = nameToken->str;
		cont = calc_expression(contToken, llvmData, moduleName, vars, errors);
	} else if (left->str == NULL) {
		funcName = get_token_string(left);
		if (funcName == NULL) {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Левая часть вызова функции не определена.");
			return kar_expression_result_none();
		}
	} else {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Левая часть вызова функции не определена.");
		return kar_expression_result_none();
	}

	// TODO: Возможно надо проверять полный путь, а не просто имя функции.
	bool is64 =
		kar_string_equal(funcName, "Целое64") ||
		kar_string_equal(funcName, "Счётное64") ||
		kar_string_equal(funcName, "Дробное32") ||
		kar_string_equal(funcName, "Дробное64");

	KAR_CREATES(argsVartree, KarVartree*, kar_token_child_count(token));
	KAR_CREATES(argsLLVM, LLVMValueRef, kar_token_child_count(token) + 1);
	size_t num = 0;
	for (size_t i = 0; i < kar_token_child_count(token); i++) {
		KarToken* child = kar_token_child_get(token , i);
		if (child->type == KAR_TOKEN_SIGN_ARGUMENT) {
			KarExpressionResult res = calc_expression(kar_token_child_get(child, 0), llvmData, moduleName, vars, errors);
			if (kar_expression_result_is_none(res)) {
				KAR_FREE(argsVartree);
				KAR_FREE(argsLLVM);
				return res;
			}
			if (is64) {
				argsVartree[num] = get_reduced64_type(res.type, vars);
			} else {
				argsVartree[num] = get_reduced_type(res.type, vars);
			}
			argsLLVM[num] = res.value;
			num++;
		}
	}

	KarString* functionName = kar_vartree_create_full_name_args(funcName, argsVartree, num);
	// TODO: Заглушка для тестов для проверки типа получаемого выражения.
	if (kar_string_equal(functionName, "ВзятьПуть()")) {
		KarString* path = kar_vartree_create_full_path(get_reduced_type(cont.type, vars));
		KarExpressionResult res = get_val_char(path, llvmData, vars);
		KAR_FREE(argsVartree);
		KAR_FREE(path);
		return res;
	}

	KarVartree* function = get_new_context(cont.type, funcName, argsVartree, num, vars);
	if (function == NULL) {
		KarString* errorText = kar_string_create_format("Не могу найти объект \"%s\".", kar_vartree_create_full_name_args(funcName, argsVartree, num));
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, errorText);
		KAR_FREE(argsVartree);
		KAR_FREE(argsLLVM);
		return kar_expression_result_none();
	}
	KAR_FREE(argsVartree);

	KarLLVMFunction* llvmFunc = kar_llvm_data_get_function(llvmData, function, vars);
	if (llvmFunc == NULL) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутренняя ошибка генератора. Не могу найти функцию.");
		KAR_FREE(argsLLVM);
		return kar_expression_result_none();
	}
	KarVartreeFunctionParams* params = kar_vartree_get_function_params(function);
	if (kar_vartree_function_is_dynamic(params->modificators)) {
		if (cont.value == NULL) {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Не могу найти класс-источник для динамической функции.");
			KAR_FREE(argsLLVM);
			return kar_expression_result_none();
		}
		for (size_t i = num; i > 0; i--) {
			argsLLVM[i] = argsLLVM[i-1];
		}
		argsLLVM[0] = cont.value;
		num++;
	}

	KarExpressionResult result;
	result.type = params->returnType;
	result.value = LLVMBuildCall(
		llvmData->builder,
		kar_llvm_function_get_ref(llvmFunc),
		argsLLVM,
		(unsigned int)num,
		params->returnType == NULL ? "" : "var"
	);
	KAR_FREE(argsLLVM);
	return result;
}

static KarVartree* getUncleanVarByType(KarVartypeElement type, KarVars* vars) {
	switch (type) {
		case KAR_VARTYPE_BOOL: return vars->standard.uncleanBool;
		case KAR_VARTYPE_INTEGER8: return vars->standard.uncleanInt8;
		case KAR_VARTYPE_INTEGER16: return vars->standard.uncleanInt16;
		case KAR_VARTYPE_INTEGER32: return vars->standard.uncleanInt32;
		case KAR_VARTYPE_INTEGER64: return vars->standard.uncleanInt64;
		case KAR_VARTYPE_UNSIGNED8: return vars->standard.uncleanUnsigned8;
		case KAR_VARTYPE_UNSIGNED16: return vars->standard.uncleanUnsigned16;
		case KAR_VARTYPE_UNSIGNED32: return vars->standard.uncleanUnsigned32;
		case KAR_VARTYPE_UNSIGNED64: return vars->standard.uncleanUnsigned64;
		case KAR_VARTYPE_FLOAT32: return vars->standard.uncleanFloat32;
		case KAR_VARTYPE_FLOAT64: return vars->standard.uncleanFloat64;
		case KAR_VARTYPE_STRING: return vars->standard.uncleanString;
		default: return NULL;
	}
	return NULL;
}

static LLVMValueRef getLLVMCleanFunctionByType(KarVartypeElement type, KarLLVMData* llvmData) {
	switch (type) {
		case KAR_VARTYPE_BOOL: return llvmData->uncleanBool;
		case KAR_VARTYPE_INTEGER8: return llvmData->uncleanInteger8;
		case KAR_VARTYPE_INTEGER16: return llvmData->uncleanInteger16;
		case KAR_VARTYPE_INTEGER32: return llvmData->uncleanInteger32;
		case KAR_VARTYPE_INTEGER64: return llvmData->uncleanInteger64;
		case KAR_VARTYPE_UNSIGNED8: return llvmData->uncleanUnsigned8;
		case KAR_VARTYPE_UNSIGNED16: return llvmData->uncleanUnsigned16;
		case KAR_VARTYPE_UNSIGNED32: return llvmData->uncleanUnsigned32;
		case KAR_VARTYPE_UNSIGNED64: return llvmData->uncleanUnsigned64;
		case KAR_VARTYPE_FLOAT32: return llvmData->uncleanFloat32;
		case KAR_VARTYPE_FLOAT64: return llvmData->uncleanFloat64;
		case KAR_VARTYPE_STRING: return llvmData->uncleanString;
		default: return NULL;
	}
	return NULL;
}

static KarExpressionResult get_sign_unclean(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* leftToken = kar_token_child_get(token, 0);
	KarExpressionResult left = calc_expression(leftToken, llvmData, moduleName, vars, errors);
	KarVartree* varType = left.type;
	if (varType == NULL) {
		kar_project_error_list_create_add(errors, moduleName, &leftToken->cursor, 1, "Невозможно определить левую часть неопределённости.");
		return kar_expression_result_none();
	}
	LLVMValueRef func = getLLVMCleanFunctionByType(get_reduced_type(varType, vars)->type, llvmData);
	if (func == NULL) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Левая часть операции неопределённости не является классом.");
		return kar_expression_result_none();
	}

	KarExpressionResult right;
	if (kar_token_child_count(token) == 1) {
		if (left.value != NULL) {
			right = left;
		} else {
			right = get_val_null(vars);
		}
	} else {
		if (left.value != NULL) {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Левая часть операции неопределённости - является значением, а не классом, при этом сеществует правая часть.");
			return kar_expression_result_none();
		}
		right = calc_expression(kar_token_child_get(token, 1), llvmData, moduleName, vars, errors);
	}
	if (kar_expression_result_is_none(right)) {
		return kar_expression_result_none();
	}

	KarExpressionResult res_clean;
	res_clean.type = get_reduced_type(varType, vars);
	res_clean.value = NULL;
	if (right.type == vars->standard.nullType) {
		res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->createPointer, &right.value, 1, "asdf");
	} else {
		bool b = check_and_cast_types(&left, &right, llvmData, vars);
		if (!b) {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Типы левой и правой части неопределённости не совпадают.");
			return kar_expression_result_none();
		}
		if (res_clean.type == vars->standard.boolType) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanBool, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.int8Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanInteger8, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.int16Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanInteger16, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.int32Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanInteger32, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.int64Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanInteger64, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.unsigned8Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanUnsigned8, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.unsigned16Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanUnsigned16, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.unsigned32Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanUnsigned32, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.unsigned64Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanUnsigned64, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.float32Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanFloat32, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.float64Type) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanFloat64, &right.value, 1, "asdf");
		} else if (res_clean.type == vars->standard.stringType) {
			res_clean.value = LLVMBuildCall(llvmData->builder, llvmData->cleanString, &right.value, 1, "asdf");
		} else {
			kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Операция неопределённости для данного типа не поддерживается.");
			return kar_expression_result_none();
		}
	}
	KarExpressionResult result;
	result.type = getUncleanVarByType(res_clean.type->type, vars);
	result.value = res_clean.value;
	return result;
}

static KarExpressionResult get_sign_clean(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* leftToken = kar_token_child_get(token, 0);
	KarExpressionResult left = calc_expression(leftToken, llvmData, moduleName, vars, errors);
	if (!left.type || left.type->type != KAR_VARTYPE_UNCLEAN_CLASS) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Левая часть операции раскрытия выражения не является неопределённостью.");
		return kar_expression_result_none();
	}

	KarVartree* function = get_new_context(vars->standard.unclean, "ПустойЛи", NULL, 0, vars);
	KarLLVMFunction* llvmFunc = kar_llvm_data_get_function(llvmData, function, vars);
	LLVMValueRef expressionValue = LLVMBuildCall(llvmData->builder, kar_llvm_function_get_ref(llvmFunc), &left.value, 1, "var");

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
	KarToken* rightToken = kar_token_child_get(token, 1);
	KarExpressionResult right = calc_expression(rightToken, llvmData, moduleName, vars, errors);
	//KarVartree* cleanLeft = kar_vartree_args_get(left.type, 0);
	KarExpressionResult cleanLeft = kar_expression_result_var(kar_vartree_args_get(left.type, 0));
	if (!check_and_cast_types(&cleanLeft, &right, llvmData, vars)) {
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "В операции раскрытия типы левой и правой части не совпадают.");
		return kar_expression_result_none();
	}
	LLVMBuildBr(llvmData->builder, mergeBlock);
	thenBlock = LLVMGetInsertBlock(llvmData->builder);

	LLVMPositionBuilderAtEnd(llvmData->builder, elseBlock);
	KarVartreeFunctionParams* params = kar_vartree_get_function_params(function);
	LLVMValueRef cleanLeftValue = LLVMBuildCall(llvmData->builder, getLLVMCleanFunctionByType(cleanLeft.type->type, llvmData), &left.value, 1, params->issueName);
	LLVMBuildBr(llvmData->builder, mergeBlock);
	elseBlock = LLVMGetInsertBlock(llvmData->builder);

	LLVMPositionBuilderAtEnd(llvmData->builder, mergeBlock);
	LLVMValueRef phi = LLVMBuildPhi(llvmData->builder, LLVMTypeOf(right.value), "ph");
	LLVMAddIncoming(phi, &right.value, &thenBlock, 1);
	LLVMAddIncoming(phi, &cleanLeftValue, &elseBlock, 1);

	KarExpressionResult result;
	result.type = cleanLeft.type;
	result.value = phi;
	return result;
}

static KarExpressionResult get_sign_single_plus(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	KarToken* child = kar_token_child_get(token, 0);
	KarExpressionResult res = calc_expression(child, llvmData, moduleName, vars, errors);
	if (res.type == vars->standard.decimalType ||
		res.type == vars->standard.hexadecimalType ||
		res.type == vars->standard.literalFloat ||
		res.type == vars->standard.int8Type ||
		res.type == vars->standard.int16Type ||
		res.type == vars->standard.int32Type ||
		res.type == vars->standard.int64Type ||
		res.type == vars->standard.unsigned8Type ||
		res.type == vars->standard.unsigned16Type ||
		res.type == vars->standard.unsigned32Type ||
		res.type == vars->standard.unsigned64Type ||
		res.type == vars->standard.float32Type ||
		res.type == vars->standard.float64Type ||

		res.type == vars->standard.uncleanInt8 ||
		res.type == vars->standard.uncleanInt16 ||
		res.type == vars->standard.uncleanInt32 ||
		res.type == vars->standard.uncleanInt64 ||
		res.type == vars->standard.uncleanUnsigned8 ||
		res.type == vars->standard.uncleanUnsigned16 ||
		res.type == vars->standard.uncleanUnsigned32 ||
		res.type == vars->standard.uncleanUnsigned64 ||
		res.type == vars->standard.uncleanFloat32 ||
		res.type == vars->standard.uncleanFloat64
	) {
		return res;
	}
	KarString* path = kar_vartree_create_full_path(get_reduced_type(res.type, vars));
	KarString* errorText = kar_string_create_format("Операция унарный плюс недопустима для типа\"%s\".", path);
	kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, errorText);
	KAR_FREE(path);
	KAR_FREE(errorText);
	return kar_expression_result_none();
}

static KarExpressionResult calc_expression(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	// TODO: Проверить на компиляторе большое количество открывающихся и закрывающихся скобок.
	switch (token->type) {

	case (KAR_TOKEN_IDENTIFIER): return get_identifier(token, moduleName, vars, errors);

	case (KAR_TOKEN_VAL_NULL): return get_val_null(vars);
	case (KAR_TOKEN_VAL_TRUE): return get_val_true(vars);
	case (KAR_TOKEN_VAL_FALSE): return get_val_false(vars);
	case (KAR_TOKEN_VAL_INTEGER): return get_val_integer(token, moduleName, vars, errors);
	case (KAR_TOKEN_VAL_HEXADECIMAL): return get_val_hexadecimal(token, moduleName, vars, errors);
	case (KAR_TOKEN_VAL_FLOAT): return get_val_float(token, moduleName, vars, errors);
	case (KAR_TOKEN_VAL_NAN): return get_val_nan(vars);
	case (KAR_TOKEN_VAL_INFINITY): return get_val_infinity(vars);
	case (KAR_TOKEN_VAL_MINUS_INFINITY): return get_val_minus_infinity(vars);
	case (KAR_TOKEN_VAL_STRING): return get_val_char(token->str, llvmData, vars);

	case (KAR_TOKEN_VAR_BOOL): return kar_expression_result_var(vars->standard.boolType);
	case (KAR_TOKEN_VAR_INTEGER8): return kar_expression_result_var(vars->standard.int8Type);
	case (KAR_TOKEN_VAR_INTEGER16): return kar_expression_result_var(vars->standard.int16Type);
	case (KAR_TOKEN_VAR_INTEGER32): return kar_expression_result_var(vars->standard.int32Type);
	case (KAR_TOKEN_VAR_INTEGER64): return kar_expression_result_var(vars->standard.int64Type);
	case (KAR_TOKEN_VAR_UNSIGNED8): return kar_expression_result_var(vars->standard.unsigned8Type);
	case (KAR_TOKEN_VAR_UNSIGNED16): return kar_expression_result_var(vars->standard.unsigned16Type);
	case (KAR_TOKEN_VAR_UNSIGNED32): return kar_expression_result_var(vars->standard.unsigned32Type);
	case (KAR_TOKEN_VAR_UNSIGNED64): return kar_expression_result_var(vars->standard.unsigned64Type);
	case (KAR_TOKEN_VAR_FLOAT32): return kar_expression_result_var(vars->standard.float32Type);
	case (KAR_TOKEN_VAR_FLOAT64): return kar_expression_result_var(vars->standard.float64Type);
	case (KAR_TOKEN_VAR_STRING): return kar_expression_result_var(vars->standard.stringType);

	case (KAR_TOKEN_SIGN_OPEN_BRACES): return get_open_braces(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_SIGN_GET_FIELD): return get_field(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_SIGN_CALL_METHOD): return get_call_method(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_SIGN_UNCLEAN): return get_sign_unclean(token, llvmData, moduleName, vars, errors);
	case (KAR_TOKEN_SIGN_CLEAN): return get_sign_clean(token, llvmData, moduleName, vars, errors);

	case (KAR_TOKEN_SIGN_SINGLE_PLUS): return get_sign_single_plus(token, llvmData, moduleName, vars, errors);
	default:
		// TODO: В сообщении об ошибке добавить тип оператора.
		kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Неизвестный оператор.");
		return kar_expression_result_none();
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
	KarExpressionResult res = calc_expression(child, llvmData, moduleName, vars, errors);
	return !kar_expression_result_is_none(res);
}
