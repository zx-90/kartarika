/* Copyright © 2021 Abdullin Timur <abdtimurrif@gmail.com>
 * Copyright © 2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
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

const KarString* KAR_FILE_SYSTEM_DELIMETER = "\\";

static LPWSTR create_utf16_by_utf8(const KarString* utf8) {
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

static KarString* create_utf8_by_utf16(LPWSTR utf16) {
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

static DWORD get_file_attributes(const KarString* path){
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		return INVALID_FILE_ATTRIBUTES;
	}
	DWORD ftyp = GetFileAttributesW(wPath);
	KAR_FREE(wPath);
	return ftyp;
}

bool kar_file_system_is_file(const KarString* path) {
	DWORD ftyp = get_file_attributes(path);
	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
		return true;
	}
	return false;
}

bool kar_file_system_is_directory(const KarString* path) {
	DWORD ftyp = get_file_attributes(path);
	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
		return true;
	}
	return false;  
}

KarString* kar_file_system_get_basename(KarString* path) {
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		return NULL;
	}

	LPCWSTR wPath2 = PathFindFileNameW(wPath);

	KarString* path2 = create_utf8_by_utf16(wPath2);
	KAR_FREE(wPath);
	return path2;
	
}

static LPWSTR create_utf16_path(const KarString* path) {
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		return NULL;
	}
	size_t wSize = wcslen(wPath);
	LPWSTR wPath2 = realloc(wPath, (wSize + 4) * sizeof(LPWSTR));
	if (wPath2 == NULL) {
		return NULL;
	}
	wPath2[wSize] = L'\\';
	wPath2[wSize + 1] = L'*';
	wPath2[wSize + 2] = L'.';
	wPath2[wSize + 3] = L'*';
	wPath2[wSize + 4] = 0;
	return wPath2;
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
KarStringList* kar_file_create_absolute_directory_list(const KarString* path) {
	
	LPWSTR wPath = create_utf16_path(path);
	if (wPath == NULL) {
		return NULL;
	}

	KarStringList* result = kar_string_list_create();
	WIN32_FIND_DATAW findData;
	HANDLE file = FindFirstFileW(wPath, &findData);
	KAR_FREE(wPath);
	if (file == INVALID_HANDLE_VALUE) {
		kar_string_list_free(result);
		FindClose(file);
		return NULL;
	}

	// TODO: Здесь надо скачивать это соединение "\\" через функции ОС.
	KarString* path2 = kar_string_create_concat(path, KAR_FILE_SYSTEM_DELIMETER);
	if (!path2) {
		kar_string_list_free(result);
		FindClose(file);
		return NULL;
	}

	do {
		if (wcscmp(findData.cFileName, L".") == 0) {
			continue;
		}
		if (wcscmp(findData.cFileName, L"..") == 0) {
			continue;
		}
		KarString* name = create_utf8_by_utf16(findData.cFileName);
		if (name == NULL) {
			kar_string_list_free(result);
			KAR_FREE(path2);
			FindClose(file);
			return NULL;
		}

		kar_string_list_add(result, kar_string_create_concat(path2, name));
		KAR_FREE(name);
	} while (FindNextFileW(file, &findData));
	KAR_FREE(path2);
	FindClose(file);
	return result;
}

FILE* kar_file_system_create_handle(KarString* path) {
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		return NULL;
	}
	HANDLE result = CreateFileW(wPath, GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	KAR_FREE(wPath);
	return result;
}

KarString* kar_file_load(const KarString* path) {
	LPWSTR wPath = create_utf16_by_utf8(path);
	if (wPath == NULL) {
		return NULL;
	}
	FILE* f = _wfopen(wPath, L"rb+");
	if (f == NULL) {
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size_t size = (size_t)ftell(f);

	fseek(f, 0, SEEK_SET);
	KAR_CREATES(result, KarString, size + 1);
	if (size != fread(result, sizeof(KarString), size, f)) {
		KAR_FREE(result);
		fclose(f);
		return NULL;
	}

	fclose(f);
	result[size] = 0;

	return result;
}

static KarString* path = NULL;

const KarString* kar_file_get_working_dir() {
	if (path) {
		return path;
	}

	WCHAR* working_dir = NULL;

	if ((working_dir = _wgetcwd(NULL, 0)) == NULL) {
		return NULL;
	}

	path = create_utf8_by_utf16(working_dir);
	free(working_dir);
	return path;
}