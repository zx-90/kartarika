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
#include <fileapi.h>
#include <shlwapi.h>

#include "core/alloc.h"
#include "core/string.h"
#include "core/error.h"

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

	LPCWSTR wPath2 = PathFindFileNameW(wPath);


	int size2 = WideCharToMultiByte(CP_UTF8, 0, wPath2, -1, NULL, 0, NULL, NULL);
	LPSTR path2 = malloc(size2 * sizeof(CHAR));
	if (!path2) {
		free(wPath);
		return NULL;
	}
	hResult = WideCharToMultiByte(CP_UTF8, 0, wPath2, -1, path2, size2, NULL, NULL);
	free(wPath);
	if (hResult == 0) {
		free(path2);
		return NULL;
	}
	return path2;
}

char** kar_file_create_absolute_directory_list(const char* path, size_t* count) {
	
	WIN32_FIND_DATAW findData;
	int wSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, NULL, 0);
	LPWSTR wPath = malloc((wSize+4) * sizeof(WCHAR));
	if (!wPath) {
		kar_error_register(1, "Ошибка выделения памяти.");
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
		(*count)++;
	} while (FindNextFileW(file, &findData));
	FindClose(file);
	if (count == 0) {

	}

	KAR_CREATES(result, char *, *count);
	file = FindFirstFileW(wPath, &findData);
	free(wPath);
	if (file == INVALID_HANDLE_VALUE) {
		KAR_FREE(result);
		kar_error_register(1, "Ошибка. Невозможно найти файлы в каталоге %s.", path);
		return NULL;
	}

	size_t number = 0;

	// TODO: Здесь надо скачивать это соединение "\\" через функции ОС.
	char* path2 = kar_string_create_concat(path, "\\");
	if (!path2) {
		KAR_FREE(result);
		*count = 0;
		return NULL;
	}

	do {
		if (wcscmp(findData.cFileName, L".") == 0) {
			continue;
		}
		if (wcscmp(findData.cFileName, L"..") == 0) {
			continue;
		}
		int pathSize = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, NULL, 0, NULL, NULL);
		KAR_CREATES(name, char, pathSize);

		hResult = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, name, pathSize, NULL, NULL);
		if (hResult == 0) {
			KAR_FREE(name);
			for (int i = 0; i < number; i++) {
				KAR_FREE(result[i]);
			}
			KAR_FREE(result);
			*count = 0;
			KAR_FREE(path2);
			return NULL;
		}

		result[number] = kar_string_create_concat(path2, name);
		KAR_FREE(name);
		number++;
	} while (FindNextFileW(file, &findData));
	KAR_FREE(path2);
	FindClose(file);
	return result;
}

FILE* kar_file_system_create_handle(char* path) {
	int wSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, NULL, 0);
	LPWSTR wPath = malloc(wSize * sizeof(WCHAR));
	if (!wPath) {
		kar_error_register(1, "Ошибка выделения памяти.");
		return NULL;
	}
	int hResult = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, wPath, wSize);
	if (hResult == 0) {
		free(wPath);
		kar_error_register(1, "Ошибка конвертации строки.");
		return NULL;
	}
	//FILE* result =  _wfopen(wPath, L"rb+");
	HANDLE result = CreateFileW(wPath, GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	free(wPath);
	return result;
}

char* kar_file_load(const char* path) {
	int wSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, NULL, 0);
	LPWSTR wPath = malloc(wSize * sizeof(WCHAR));
	if (!wPath) {
		kar_error_register(1, "Ошибка выделения памяти.");
		return NULL;
	}
	int hResult = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, path, -1, wPath, wSize);
	if (hResult == 0) {
		free(wPath);
		kar_error_register(1, "Ошибка конвертации строки.");
		return NULL;
	}
	FILE* f = _wfopen(wPath, L"rb+");
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

	return result;
}

static LPSTR path = NULL;

const char* kar_file_get_working_dir() {
	if (path) {
		return path;
	}

	WCHAR* working_dir = NULL;

	if ((working_dir = _wgetcwd(NULL, 0)) == NULL) {
		return NULL;
	}

	int size = WideCharToMultiByte(CP_UTF8, 0, working_dir, -1, NULL, 0, NULL, NULL);
	path = malloc(size * sizeof(CHAR));
	if (!path) {
		free(working_dir);
		return NULL;
	}
	int hResult = WideCharToMultiByte(CP_UTF8, 0, working_dir, -1, path, size, NULL, NULL);//Меняем строку кодировки ютф16 на ютф8
	free(working_dir);
	if (hResult == 0) {
		free(path);
		return NULL;
	}
	return path;
}