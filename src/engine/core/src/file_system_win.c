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

const char* KAR_FILE_SYSTEM_DELIMETER = "\\";

static LPWSTR create_utf16_by_utf8(const char* utf8) {
	int wSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, utf8, -1, NULL, 0);
	KAR_CREATES(utf16, WCHAR, wSize);
	if (!utf16) {
		return NULL;
	}
	int hResult = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, utf8, -1, utf16, wSize);
	if (hResult == 0) {
		KAR_FREE(utf16);
		return NULL;
	}
	return utf16;
}

static char* create_utf8_by_utf16(LPWSTR utf16) {
	int size = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL);
	KAR_CREATES(utf8, CHAR, size);
	if (!utf8) {
		return NULL;
	}
	int hResult = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, utf8, size, NULL, NULL);
	if (hResult == 0) {
		KAR_FREE(utf8);
		return NULL;
	}
	return utf8;
}

static DWORD get_file_attributes(const char* path){
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		return INVALID_FILE_ATTRIBUTES;
	}
	DWORD ftyp = GetFileAttributesW(wPath);
	KAR_FREE(wPath);
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
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		return NULL;
	}

	LPCWSTR wPath2 = PathFindFileNameW(wPath);

	char* path2 = create_utf8_by_utf16(wPath2);
	KAR_FREE(wPath);
	return path2;
	
}

static LPWSTR create_utf16_path(const char* path) {
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		return NULL;
	}
	int wSize = wcslen(wPath);
	wPath = realloc(wPath, (wSize + 4) * sizeof(LPWSTR));
	wPath[wSize] = L'\\';
	wPath[wSize + 1] = L'*';
	wPath[wSize + 2] = L'.';
	wPath[wSize + 3] = L'*';
	wPath[wSize + 4] = 0;
	return wPath;
}

static size_t get_file_count(LPWSTR wPath) {
	WIN32_FIND_DATAW findData;
	HANDLE file = FindFirstFileW(wPath, &findData);
	if (file == INVALID_HANDLE_VALUE) {
		return -1;
	}

	int count = 0;
	do {
		if (wcscmp(findData.cFileName, L".") == 0) {
			continue;
		}
		if (wcscmp(findData.cFileName, L"..") == 0) {
			continue;
		}
		count++;
	} while (FindNextFileW(file, &findData));
	FindClose(file);
	return count;
}
char** kar_file_create_absolute_directory_list(const char* path, size_t* count) {
	
	LPWSTR wPath = create_utf16_path(path);
	if (wPath == NULL) {
		kar_error_register(1, "Ошибка выделения памяти.");
		return NULL;
	}

	*count = get_file_count(wPath);
	if (*count == -1) {
		KAR_FREE(wPath);
		kar_error_register(1, "Ошибка. Невозможно найти файлы в каталоге %s.", path);
		return NULL;
	}
	if (*count == 0) {
		KAR_FREE(wPath);
		KAR_CREATES(result, char*, 0);
		return result;
	}

	WIN32_FIND_DATAW findData;
	KAR_CREATES(result, char *, *count);
	HANDLE file = FindFirstFileW(wPath, &findData);
	KAR_FREE(wPath);
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
		char* name = create_utf8_by_utf16(findData.cFileName);
		if (name == NULL) {
			kar_string_list_free(result, number);
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
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		kar_error_register(1, "Ошибка выделения памяти.");
		return NULL;
	}
	HANDLE result = CreateFileW(wPath, GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	KAR_FREE(wPath);
	return result;
}

char* kar_file_load(const char* path) {
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		kar_error_register(1, "Ошибка выделения памяти.");
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

	char* path = create_utf8_by_utf16(working_dir);
	free(working_dir);
	return path;
}