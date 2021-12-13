/* Copyright © 2020,2021 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_FILE_SYSTEM_H
#define KAR_FILE_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

extern const char* KAR_FILE_SYSTEM_DELIMETER;

bool kar_file_system_is_file(const char* path);
bool kar_file_system_is_directory(const char* path);

char* kar_file_system_get_basename(char* path);

char** kar_file_create_absolute_directory_list(const char* path, size_t* count);

FILE* kar_file_system_create_handle(char* path);

char* kar_file_load(const char* path);

const char* kar_file_get_working_dir();

#endif // KAR_FILE_SYSTEM_H
