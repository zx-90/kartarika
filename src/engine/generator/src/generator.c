/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "generator/generator.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

#include <llvm-c/BitWriter.h>

static bool generate_identifier(const KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder) {
	if (token->type != KAR_TOKEN_INDENT) {
		return false;
	}
	const KarToken* child1 = kar_token_child(token, 0);
	if (!kar_token_check_type_name(child1, KAR_TOKEN_IDENTIFIER, "Кар")) {
		return false;
	}
	const KarToken* child2 = kar_token_child(child1, 0);
	if (!kar_token_check_type(child2, KAR_TOKEN_SIGN_GET_FIELD)) {
		return false;
	}
	const KarToken* child3 = kar_token_child(child2, 0);
	if (!kar_token_check_type_name(child3, KAR_TOKEN_IDENTIFIER, "Печатать")) {
		return false;
	}
	const KarToken* child4 = kar_token_child(child3, 0);
	if (!kar_token_check_type(child4, KAR_TOKEN_SIGN_OPEN_BRACES)) {
		return false;
	}
	const KarToken* child5 = kar_token_child(child4, 0);
	if (!kar_token_check_type(child5, KAR_TOKEN_VAL_STRING)) {
		return false;
	}
	
	LLVMTypeRef type = LLVMPointerType(LLVMInt8Type(), 0);
	LLVMTypeRef puts_type = LLVMFunctionType(LLVMInt32Type(), &type, 1, false);
	LLVMValueRef puts_func = LLVMAddFunction(module, "puts", puts_type);
	
	LLVMValueRef out_string = LLVMBuildGlobalStringPtr(builder, child5->str, "helloWorld");
	LLVMBuildCall(builder, puts_func, &out_string, 1, "puts");
	
	return true;
}
	
static bool generate_function(KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder) {
	if (token->type != KAR_TOKEN_METHOD) {
		return false;
	}
	if (!strcmp(token->str, "запустить")) {
		LLVMTypeRef func_type = LLVMFunctionType(LLVMVoidType(), NULL, 0, false);
		LLVMValueRef main_func = LLVMAddFunction(module, "main", func_type);
		LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main_func, "entry");
		LLVMPositionBuilderAtEnd(builder, entry);
	
		for (size_t i = 0; i < token->children.count; ++i) {
			generate_identifier(kar_token_child(token, i), module, builder);
		}
		LLVMBuildRetVoid(builder);
	} else {
		// Далее здесь необходимо дописать поддержку других методов.
		return false;
	}
	return true;
}
	
static bool generate_module(const KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder) {
	if (token->type != KAR_TOKEN_MODULE) {
		return false;
	}
	for (size_t i = 0; i < token->children.count; ++i) {
		if (kar_token_child(token, i)->type == KAR_TOKEN_METHOD) {
			generate_function(kar_token_child(token, i), module, builder);
		} else {
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

bool kar_generator_run(KarModule* mod) {
	// TODO: обработка ошибок. Добавить.
	LLVMContextRef context = LLVMContextCreate();
	LLVMModuleRef module = LLVMModuleCreateWithNameInContext("asdf", context);
	LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
	
	generate_module(mod->token, module, builder);
	
	// generate_hello_world(module, builder);
	
	LLVMDisposeBuilder(builder);
	
	// TODO: Инициализации в отдельную функцию надо будет выделить.
	// Initialize the target registry etc.
	LLVMInitializeAllTargetInfos();
	LLVMInitializeAllTargets();
	LLVMInitializeAllTargetMCs();
	LLVMInitializeAllAsmParsers();
	LLVMInitializeAllAsmPrinters();

	char* target_triple = LLVMGetDefaultTargetTriple();
	LLVMSetTarget(module, target_triple);

	LLVMTargetRef target;
	char* error;
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
	system("clang-9 asdf.o -o a.out");
	return true;
}
