/* Copyright © 2020-2022 Evgeny Zaytsev <zx_90@mail.ru>
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

#include "core/error.h"
#include "core/string.h"
#include "core/alloc.h"

static bool print(const char* out, LLVMModuleRef module, LLVMBuilderRef builder) {
	LLVMTypeRef type = LLVMPointerType(LLVMInt8Type(), 0);
	LLVMTypeRef puts_type = LLVMFunctionType(LLVMInt32Type(), &type, 1, false);
	// TODO: Проверить, возможно printf будет неправильно работать с % (По аналогии с языком C/C++).
	// TODO: Выдает ошибку если вызвать Кар.Печатать 2 раза
	LLVMValueRef puts_func = LLVMAddFunction(module, "printf", puts_type);
	
	LLVMValueRef out_string = LLVMBuildGlobalStringPtr(builder, out, "helloWorld");
	LLVMBuildCall(builder, puts_func, &out_string, 1, "printf");
	
	return true;
}

static bool generate_identifier(const KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder) {
	if (token->type != KAR_TOKEN_COMMAND_EXPRESSION) {
		// TODO: Здесь и дальше вместо printf-error необходимо добавить описания ошибок в kar_module_error.
		printf("ERROR 1\n");
		return false;
	}
	const KarToken* child1 = kar_token_child(token, 0);
	if (!kar_token_check_type(child1, KAR_TOKEN_SIGN_GET_FIELD)) {
		printf("ERROR 2\n");
		return false;
	}
	const KarToken* child2 = kar_token_child(child1, 0);
	if (!kar_token_check_type_name(child2, KAR_TOKEN_IDENTIFIER, "Кар")) {
		printf("ERROR 3\n");
		return false;
	}
	const KarToken* child3 = kar_token_child(child1, 1);
	if (!kar_token_check_type(child3, KAR_TOKEN_SIGN_CALL_METHOD)) {
		printf("ERROR 4\n");
		return false;
	}
	const KarToken* child4a = kar_token_child(child3, 0);
	if (!kar_token_check_type_name(child4a, KAR_TOKEN_IDENTIFIER, "Печатать")) {
		printf("ERROR 5\n");
		return false;
	}
	const KarToken* child4 = kar_token_child(child3, 1);
	if (!kar_token_check_type(child4, KAR_TOKEN_SIGN_ARGUMENT)) {
		printf("ERROR 6\n");
		return false;
	}
	const KarToken* child5 = kar_token_child(child4, 0);
	
	if (kar_token_check_type(child5, KAR_TOKEN_VAL_TRUE)) {
		return print("Да", module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_FALSE)) {
		return print("Нет", module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_INTEGER)) {
		// TODO: Эту проверку необходимо перенести в анализатор.
		// TODO: Проверку надо более тщательно организовать. Сейчас работает только проверка на длину строки.
		if (strlen(child5->str) > 11) {
			printf("ERROR 7\n");
			return false;
		}
		int32_t val;
		if (1 != sscanf(child5->str, "%"SCNd32, &val)) {
			printf("ERROR 8\n");
			return false;
		}
		return print(child5->str, module, builder);
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_HEXADECIMAL)) {
		// TODO: Эту проверку необходимо перенести в анализатор.
		if (strlen(child5->str) > 7) {
			printf("ERROR 9\n");
			return false;
		}
		uint32_t val;
		if (1 != sscanf(child5->str + 3, "%x", &val)) {
			printf("ERROR 10\n");
			return false;
		}
		char textToWrite[16];
		sprintf(textToWrite,"%"SCNu32, val);
		
		char* out_chars = kar_string_create_copy(textToWrite);
		bool result = print(out_chars, module, builder);
		KAR_FREE(out_chars);
		return result;
	} else if (kar_token_check_type(child5, KAR_TOKEN_VAL_FLOAT)) {
		double d;
		// TODO: Доделать все особые случаи.
		if (!strcmp(child5->str, "НеЧисло")) {
			d = NAN;
		} else {
			char* tmp1 = kar_string_create_replace(child5->str, ",", ".");
			char* tmp2 = kar_string_create_replace(tmp1, "с", "E");
			char* tmp3 = kar_string_create_replace(tmp2, "С", "E");
			d = strtod(tmp3, NULL);
			KAR_FREE(tmp1);
			KAR_FREE(tmp2);
			KAR_FREE(tmp3);
			if (isnan(d) || isinf(d)) {
				printf("ERROR 11\n");
				return false;
			}
		}
		
		if (errno == ERANGE) {
			printf("ERROR 12\n");
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
			char output[50];
			snprintf(output, 50, "%f", d);
			char* tmp4 = kar_string_create_replace(output, ".", ",");
			bool result = print(tmp4, module, builder);
			KAR_FREE(tmp4);
			return result;
		} else {
			char output[50];
			snprintf(output, 50, "%.4e", d);
			char* tmp4 = kar_string_create_replace(output, ".", ",");
			char* tmp5 = kar_string_create_replace(tmp4, "e", "с");
			bool result = print(tmp5, module, builder);
			KAR_FREE(tmp4);
			KAR_FREE(tmp5);
			return result;
		}
		
		printf("ERROR 13\n");
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
	
		printf("ERROR 14\n");
	return false;
}
	
static bool generate_function(KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder) {
	if (token->type != KAR_TOKEN_METHOD) {
		return false;
	}
	if (!strcmp(token->str, "Запустить")) {
		LLVMTypeRef func_type = LLVMFunctionType(LLVMVoidType(), NULL, 0, false);
		LLVMValueRef main_func = LLVMAddFunction(module, "main", func_type);
		LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main_func, "entry");
		LLVMPositionBuilderAtEnd(builder, entry);

		KarToken* body = kar_token_child(token, token->children.count - 1);
	
		for (size_t i = 0; i < body->children.count; ++i) {
			if (!generate_identifier(kar_token_child(body, i), module, builder)) {
				LLVMBuildRetVoid(builder);
				return false;
			}
		}
		LLVMBuildRetVoid(builder);
	} else {
		// Далее здесь необходимо дописать поддержку других методов.
		printf("ERROR 15\n");
		return false;
	}
	return true;
}
	
static bool generate_module(const KarToken* token, LLVMModuleRef module, LLVMBuilderRef builder) {
	if (token->type != KAR_TOKEN_MODULE) {
		printf("ERROR 16\n");
		return false;
	}
	for (size_t i = 0; i < token->children.count; ++i) {
		if (kar_token_child(token, i)->type == KAR_TOKEN_METHOD) {
			if (!generate_function(kar_token_child(token, i), module, builder)) {
				return false;
			}
		} else {
			printf("ERROR 17\n");
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
bool kar_generator_run(KarModule* mod) {
	// TODO: обработка ошибок. Добавить.
	// TODO: Windows настроить кодировку консоли.
	// TODO: Откомпилированная программа выдает лишнюю строчку в консоль.
	LLVMContextRef context = LLVMContextCreate();
	LLVMModuleRef module = LLVMModuleCreateWithNameInContext("asdf", context);
	LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
	
	if (!generate_module(mod->token, module, builder)) {
		kar_error_register(1, "Ошибка в генераторе.");
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
#ifdef __linux__
	system("clang-9 asdf.o -o a.out");
#elif _WIN32
	system("B:\\llvmexe\\LLVM\\bin\\clang.exe asdf.o -o a.exe");
	/*system("B:\\llvmexe\\LLVM\\bin\\clang.exe link.exe /ENTRY:main asdf.o");*/
#endif
	return true;
}
