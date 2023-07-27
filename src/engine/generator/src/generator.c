/* Copyright © 2020-2023 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021,2022 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/generator.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/BitWriter.h>

#include "core/string.h"
#include "core/alloc.h"

LLVMValueRef puts_func = NULL;

static bool print(const KarString* out, LLVMModuleRef module, LLVMBuilderRef builder) {
    if (puts_func == NULL) {
        LLVMTypeRef type = LLVMPointerType(LLVMInt8Type(), 0);
        LLVMTypeRef puts_type = LLVMFunctionType(LLVMInt32Type(), &type, 1, false);
        puts_func = LLVMAddFunction(module, "_kartarika_library_write_chars", puts_type);
    }
	
	LLVMValueRef out_string = LLVMBuildGlobalStringPtr(builder, out, "helloWorld");
    LLVMBuildCall(builder, puts_func, &out_string, 1, "_kartarika_library_write_chars");
	
	return true;
}

static bool generate_identifier(KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_COMMAND_EXPRESSION) {
        kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Токен не является командой.");
		return false;
	}
    KarToken* child1 = kar_token_child_get(token, 0);
	if (!kar_token_check_type(child1, KAR_TOKEN_SIGN_GET_FIELD)) {
        kar_project_error_list_create_add(errors, moduleName, &child1->cursor, 1, "Внутренняя ошибка. Токен команды не имеет потомков.");
        return false;
	}
    KarToken* child2 = kar_token_child_get(child1, 0);
    if (!kar_token_check_type_name(child2, KAR_TOKEN_IDENTIFIER, "Консоль")) {
        kar_project_error_list_create_add(errors, moduleName, &child2->cursor, 1, "Поддерживается только модуль \"Консоль\".");
        return false;
	}
    KarToken* child3 = kar_token_child_get(child1, 1);
	if (!kar_token_check_type(child3, KAR_TOKEN_SIGN_CALL_METHOD)) {
        kar_project_error_list_create_add(errors, moduleName, &child3->cursor, 1, "В модуле \"Консоль\" поддерживается только вызов метода.");
        return false;
	}
    KarToken* child4a = kar_token_child_get(child3, 0);
    if (!kar_token_check_type_name(child4a, KAR_TOKEN_IDENTIFIER, "Вывод")) {
        kar_project_error_list_create_add(errors, moduleName, &child4a->cursor, 1, "Поддерживается только метод \"Консоль.Вывод()\".");
        return false;
	}
    KarToken* child4 = kar_token_child_get(child3, 1);
	if (!kar_token_check_type(child4, KAR_TOKEN_SIGN_ARGUMENT)) {
        kar_project_error_list_create_add(errors, moduleName, &child4->cursor, 1, "Отсутствуют аргументы у метода \"Консоль.Вывод()\".");
        return false;
	}
    KarToken* child5 = kar_token_child_get(child4, 0);
	
	if (kar_token_check_type(child5, KAR_TOKEN_VAL_TRUE)) {
        KarString* functionName = kar_vartree_create_full_function_name(child4a->str, &vars->standard.boolType, 1);
        KarVartree* console = kar_vars_find(vars, child2->str);
        if (console == NULL) {
            kar_project_error_list_create_add(errors, moduleName, &child2->cursor, 1, "Не могу найти объект \"Консоль\".");
            KAR_FREE(functionName);
            return false;
        }
        KarVartree* function = kar_vartree_find(console, functionName);
        if (function == NULL) {
            kar_project_error_list_create_add(errors, moduleName, &child2->cursor, 1, "Не могу найти объект \"Консоль.Вывод(Кар.Типы.Буль)\".");
            KAR_FREE(console);
            KAR_FREE(functionName);
            return false;
        }

		return print("Да", module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_FALSE)) {
		return print("Нет", module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_INTEGER)) {
		// TODO: Эту проверку необходимо перенести в анализатор.
		// TODO: Проверку надо более тщательно организовать. Сейчас работает только проверка на длину строки.
		if (strlen(child5->str) > 11) {
            kar_project_error_list_create_add(errors, moduleName, &child5->cursor, 1, "Слишком большое число.");
            return false;
		}
		int32_t val;
		if (1 != sscanf(child5->str, "%"SCNd32, &val)) {
            kar_project_error_list_create_add(errors, moduleName, &child5->cursor, 1, "Не корректное число.");
            return false;
		}
		return print(child5->str, module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_HEXADECIMAL)) {
		// TODO: Эту проверку необходимо перенести в анализатор.
		if (strlen(child5->str) > 7) {
            kar_project_error_list_create_add(errors, moduleName, &child5->cursor, 1, "Слишком большое шестнадцатеричное число.");
            return false;
		}
		uint32_t val;
		if (1 != sscanf(child5->str + 3, "%x", &val)) {
            kar_project_error_list_create_add(errors, moduleName, &child5->cursor, 1, "Не корректное шестнадцатеричное число.");
            return false;
		}
		KarString textToWrite[16];
		sprintf(textToWrite,"%"SCNu32, val);
		
		KarString* out_chars = kar_string_create(textToWrite);
		bool result = print(out_chars, module, builder);
		KAR_FREE(out_chars);
		return result;
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_FLOAT)) {
		double d;
		// TODO: Доделать все особые случаи.
		if (kar_string_equal(child5->str, "НеЧисло")) {
			d = NAN;
		} else {
			KarString* tmp1 = kar_string_create_replace(child5->str, ",", ".");
			KarString* tmp2 = kar_string_create_replace(tmp1, "с", "E");
			KarString* tmp3 = kar_string_create_replace(tmp2, "С", "E");
			d = strtod(tmp3, NULL);
			KAR_FREE(tmp1);
			KAR_FREE(tmp2);
			KAR_FREE(tmp3);
			if (isnan(d) || isinf(d)) {
                kar_project_error_list_create_add(errors, moduleName, &child5->cursor, 1, "Не корректное дробное число.");
                return false;
			}
		}
		
		if (errno == ERANGE) {
            kar_project_error_list_create_add(errors, moduleName, &child5->cursor, 1, "Не корректное дробное число.");
            return false;
		}
		
		double absd = fabs(d);
		// TODO: Доделать все особые случаи.
		if (isnan(d)) {
			return print("НеЧисло", module, builder);
		} else if (isinf(d) && d > 0.0) {
			return print("∞", module, builder);
		} else if (isinf(d) && d < 0.0) {
			return print("-∞", module, builder);
		} else if (absd >= 0.0001 && absd <= 1000.0) {
			KarString output[50];
			snprintf(output, 50, "%f", d);
			KarString* tmp4 = kar_string_create_replace(output, ".", ",");
			bool result = print(tmp4, module, builder);
			KAR_FREE(tmp4);
			return result;
		} else {
			KarString output[50];
			snprintf(output, 50, "%.4e", d);
			KarString* tmp4 = kar_string_create_replace(output, ".", ",");
			KarString* tmp5 = kar_string_create_replace(tmp4, "e", "с");
			bool result = print(tmp5, module, builder);
			KAR_FREE(tmp4);
			KAR_FREE(tmp5);
			return result;
		}
		
        kar_project_error_list_create_add(errors, moduleName, &child5->cursor, 1, "Не корректное дробное число.");
        return false;
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_NAN)) {
		return print("НеЧисло", module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_INFINITY)) {
		return print("∞", module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_MINUS_INFINITY)) {
		return print("-∞", module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_STRING)) {
		return print(child5->str, module, builder);
	}
	
    kar_project_error_list_create_add(errors, moduleName, &child5->cursor, 1, "Тип аргумента функции не соответствует ожидаемому.");
    return false;
}
	
static bool generate_function(KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_METHOD) {
		return false;
	}
	if (kar_string_equal(token->str, "Запустить")) {
		LLVMTypeRef func_type = LLVMFunctionType(LLVMVoidType(), NULL, 0, false);
		LLVMValueRef main_func = LLVMAddFunction(module, "main", func_type);
		LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main_func, "entry");
		LLVMPositionBuilderAtEnd(builder, entry);

		KarToken* body = kar_token_child_get_last(token, 0);
	
		for (size_t i = 0; i < kar_token_child_count(body); ++i) {
            if (!generate_identifier(kar_token_child_get(body, i), module, builder, moduleName, vars, errors)) {
				LLVMBuildRetVoid(builder);
				return false;
			}
		}
		LLVMBuildRetVoid(builder);
	} else {
		// Далее здесь необходимо дописать поддержку других методов.
        kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Методы не поддерживаются. Поддерживается только метод \"Запустить\".");
		return false;
	}
	return true;
}
	
static bool generate_module(KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_MODULE) {
        kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутрення ошибка. Токен не является типом модуль.");
		return false;
	}
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
        KarToken* child = kar_token_child_get(token, i);
        if (child->type == KAR_TOKEN_METHOD) {
            if (!generate_function(kar_token_child_get(token, i), module, builder, moduleName, vars, errors)) {
				return false;
			}
		} else {
            kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Внутрення ошибка. Токен не является корневым элементом.");
            return false;
		}
	}
	return true;
}

/*static void generate_hello_world(LLVMModuleRef module, LLVMBuilderRef builder) {
	LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), NULL, 0, false);
	LLVMValueRef mainFunc = LLVMAddFunction(module, "main", funcType);
	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(mainFunc, "entry");
	LLVMPositionBuilderAtEnd(builder, entry);
	
	LLVMTypeRef type = LLVMPointerType(LLVMInt8Type(), 0);
	LLVMTypeRef putsType = LLVMFunctionType(LLVMInt32Type(), &type, 1, false);
	LLVMValueRef putsFunc = LLVMAddFunction(module, "puts", putsType);
	
	LLVMValueRef helloWorld = LLVMBuildGlobalStringPtr(builder, "Привет, мир!!!", "helloWorld");
	LLVMBuildCall(builder, putsFunc, &helloWorld, 1, "puts");

	LLVMBuildRetVoid(builder);
}*/

// TODO: Надо передавать KarProject вместо KarModule. 
bool kar_generator_run(KarProject* project) {
    puts_func = NULL;
	// TODO: обработка ошибок. Добавить.
	// TODO: Windows настроить кодировку консоли.
	// TODO: Откомпилированная программа выдает лишнюю строчку в консоль.
	LLVMContextRef context = LLVMContextCreate();
	LLVMModuleRef module = LLVMModuleCreateWithNameInContext("asdf", context);
	LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
	
    KarModule* mod = project->module;
    if (!generate_module(mod->token, module, builder, mod->name, project->vars, project->errors)) {
        kar_project_error_list_create_add(project->errors, mod->name, &mod->token->cursor, 1, "Ошибка в генераторе.");
		return false;
	}
	
	// generate_hello_world(module, builder);
	
	LLVMDisposeBuilder(builder);
	
	// TODO: Инициализации в отдельную функцию надо будет выделить.
	// Initialize the target registry etc.
	LLVMInitializeAllTargetInfos();
	LLVMInitializeAllTargets();
	LLVMInitializeAllTargetMCs();
	LLVMInitializeAllAsmParsers();
	LLVMInitializeAllAsmPrinters();

	KarString* target_triple = LLVMGetDefaultTargetTriple();
	LLVMSetTarget(module, target_triple);

	LLVMTargetRef target;
	KarString* error;
	if (LLVMGetTargetFromTriple(target_triple, &target, &error)) {
		printf("%s\n", error);
		LLVMDisposeMessage(error);
		LLVMDisposeMessage(target_triple);
		LLVMDisposeModule(module);
		LLVMContextDispose(context);
		return false;
	}

	LLVMCodeGenOptLevel opt = LLVMCodeGenLevelNone;
	LLVMRelocMode rm = LLVMRelocDefault;
	
	LLVMTargetMachineRef the_target_machine =
		LLVMCreateTargetMachine(target, target_triple, "generic", "", opt, rm, LLVMCodeModelDefault);

	LLVMTargetDataRef target_data = LLVMCreateTargetDataLayout(the_target_machine);
	LLVMSetModuleDataLayout(module, target_data);

	if (LLVMTargetMachineEmitToFile(the_target_machine, module, "asdf.o", LLVMObjectFile, &error)) {
		printf("%s\n", error);
		LLVMDisposeMessage(error);
		LLVMDisposeTargetData(target_data);
		LLVMDisposeTargetMachine(the_target_machine);
		LLVMDisposeMessage(target_triple);
		LLVMDisposeModule(module);
		LLVMContextDispose(context);
		return false;
	}

	/*if (LLVMPrintModuleToFile(module, "b.out", &error)) {
		printf("%s\n", error);
		LLVMDisposeMessage(error);
		LLVMDisposeTargetData(target_data);
		LLVMDisposeTargetMachine(the_target_machine);
		LLVMDisposeMessage(target_triple);
		LLVMDisposeModule(module);
		LLVMContextDispose(context);
		return false;
	}*/
	
	LLVMDisposeTargetData(target_data);
	LLVMDisposeTargetMachine(the_target_machine);
	LLVMDisposeMessage(target_triple);
	LLVMDisposeModule(module);
	LLVMContextDispose(context);
	
	// TODO: Разобраться можно ли это как-то без clang делать. Только с помощью llvm.
#ifdef __linux__
    bool result = system("clang-9 asdf.o library.o -o a.out") == 0;
#elif _WIN32
    bool result = system("clang.exe asdf.o library.o -o a.exe") == 0;
	/*system("B:\\llvmexe\\LLVM\\bin\\clang.exe link.exe /ENTRY:main asdf.o");*/
#endif
	return result;
}
