/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 *
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "core/file_system.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <windows.h>

#include "core/alloc.h"
#include "core/string.h"
#include "core/error.h"

#include <shlwapi.h>

static DWORD get_file_attributes(const char* path){
	int wSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, NULL, 0);
	LPWSTR wPath = malloc(wSize * sizeof(WCHAR));
	if (!wPath) {
		return INVALID_FILE_ATTRIBUTES;
	}
	int hResult = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, wPath, wSize);
	if (hResult == 0) {
		free(wPath);
		return INVALID_FILE_ATTRIBUTES;
	}
	DWORD ftyp = GetFileAttributesW(wPath);
	free(wPath);
	return ftyp;
}

bool kar_file_system_is_file(const char* path) {
	DWORD ftyp = get_file_attributes(path);
	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
		return true;
	}
	return false;
}

bool kar_file_system_is_directory(const char* path) {
	DWORD ftyp = get_file_attributes(path);
	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
		return true;
	}
	return false;  
}

char* kar_file_system_get_basename(char* path) {
	int wSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, NULL, 0);
	LPWSTR wPath = malloc(wSize * sizeof(WCHAR));
	if (!wPath) {
		return NULL;
	}
	int hResult = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, wPath, wSize);
	if (hResult == 0) {
		free(wPath);
		return NULL;
	}

	LPWSTR wPath_2 = (LPWSTR)PathFindFileNameA(wPath);
	//return basename(path);
	return (char *)wPath_2;
}

char** kar_file_create_absolute_directory_list(const char* path, size_t* count) {
	
	WIN32_FIND_DATAW findData;
	int wSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, NULL, 0);
	LPWSTR wPath = malloc((wSize+4) * sizeof(WCHAR));
	if (!wPath) {
		kar_error_register(1, "Ошибка выделения памяти.");//
		return NULL;
	}
	int hResult = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, wPath, wSize);
	if (hResult == 0) {
		free(wPath);
		kar_error_register(1, "Ошибка конвертации строки.");
		return NULL;
	}
	wSize = wcslen(wPath);
	wPath[wSize] = L'\\';
	wPath[wSize+1] = L'*';
	wPath[wSize+2] = L'.';
	wPath[wSize+3] = L'*';
	wPath[wSize+4] = 0;

	HANDLE file = FindFirstFileW(wPath, &findData);

	if (file == INVALID_HANDLE_VALUE) {
		free(wPath);
		kar_error_register(1, "Ошибка. Невозможно найти файлы в каталоге %s.", path);
		return NULL;
	}

	*count = 0;
	do {
		if (wcscmp(findData.cFileName, L".") == 0) {
			continue;
		}
		if (wcscmp(findData.cFileName, L"..") == 0) {
			continue;
		}
		*count++;
	} while (FindNextFileW(file, &findData));
	FindClose(file);

	KAR_CREATES(result, char *, *count);
	file = FindFirstFileW(wPath, &findData);
	free(wPath);
	if (file == INVALID_HANDLE_VALUE) {
		KAR_FREE(result);
		kar_error_register(1, "Ошибка. Невозможно найти файлы в каталоге %s.", path);
		return NULL;
	}

	size_t number = 0;
	do {
		if (wcscmp(findData.cFileName, L".") == 0) {
			continue;
		}
		if (wcscmp(findData.cFileName, L"..") == 0) {
			continue;
		}
		int pathSize = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, NULL, 0, NULL, NULL);
		KAR_CREATES(path, char, pathSize);
		result[number] = path;
		number++;
	} while (FindNextFileW(file, &findData));
	FindClose(file);
	/*char** result = kar_file_create_directory_list(path, count);
	if (!result) {
		return NULL;
	}

	// TODO: Здесь надо скачивать это соединение "/" через функции ОС.
	char* path2 = kar_string_create_concat(path, "/");
	if (!path2) {
		kar_string_list_free(result, *count);
		*count = 0;
		return NULL;
	}

	size_t i;
	for (i = 0; i < *count; i++) {
		char* file_name = result[i];
		char* absolute_file_name = kar_string_create_concat(path2, file_name);
		KAR_FREE(result[i]);
		result[i] = absolute_file_name;
	}
	KAR_FREE(path2);
	return result;*/
	return result;
}

char* kar_file_load(const char* path) {
	/*FILE* f = fopen(path, "rb");
	if (f == NULL) {
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size_t size = (size_t)ftell(f);

	fseek(f, 0, SEEK_SET);
	KAR_CREATES(result, char, size + 1);
	if (size != fread(result, sizeof(char), size, f)) {
		KAR_FREE(result);
		fclose(f);
		return NULL;
	}

	fclose(f);
	result[size] = 0;

	return result;*/
	return NULL;
}

static char* working_dir = NULL;

const char* kar_file_get_working_dir() {
	/*if (working_dir) {
		return working_dir;
	}
	working_dir = getcwd(NULL, 0);*/
	return working_dir;
}