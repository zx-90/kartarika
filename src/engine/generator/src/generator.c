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
#include "model/vartree_function_params.h"
#include "generator/llvm_data.h"
#include "generator/gen_expression.h"

static bool generate_commands(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
		KarToken* child = kar_token_child_get(token, i);
		if (child->type == KAR_TOKEN_COMMAND_EXPRESSION) {
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
		} else {
			kar_project_error_list_create_add(errors, moduleName, &child->cursor, 1, "Токен не является командой.");
			LLVMBuildRetVoid(llvmData->builder);
			return false;
		}
	}
	return true;
}

static bool generate_function(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
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
		if (!generate_commands(body, llvmData, moduleName, vars, errors)) {
			return false;
		}
		kar_local_stack_block_erase(vars->locals, 0);
		LLVMBuildRetVoid(llvmData->builder);
	} else {
		// Далее здесь необходимо дописать поддержку других методов.
        kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Методы не поддерживаются. Поддерживается только метод \"Запустить\".");
		return false;
	}
	return true;
}
	
static bool generate_module(KarToken* token, KarLLVMData* llvmData, KarString* moduleName, KarVars* vars, KarProjectErrorList* errors) {
	if (token->type != KAR_TOKEN_MODULE) {
        kar_project_error_list_create_add(errors, moduleName, &token->cursor, 1, "Внутрення ошибка. Токен не является типом модуль.");
		return false;
	}
	for (size_t i = 0; i < kar_token_child_count(token); ++i) {
        KarToken* child = kar_token_child_get(token, i);
        if (child->type == KAR_TOKEN_METHOD) {
			if (!generate_function(kar_token_child_get(token, i), llvmData, moduleName, vars, errors)) {
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

bool kar_generator_run(KarProject* project) {
	// TODO: Windows настроить кодировку консоли.
	LLVMContextRef context = LLVMContextCreate();
	LLVMModuleRef module = LLVMModuleCreateWithNameInContext("asdf", context);
	LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
	
    KarModule* mod = project->module;
	KarLLVMData* llvmData = kar_llvm_data_create(context, module, builder);
	if (!generate_module(mod->token, llvmData, mod->name, project->vars, project->errors)) {
        kar_project_error_list_create_add(project->errors, mod->name, &mod->token->cursor, 1, "Ошибка в генераторе.");
		return false;
	}
	kar_llvm_data_free(llvmData);
	
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
