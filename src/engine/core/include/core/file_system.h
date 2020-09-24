/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>

bool k_file_system_is_file(const char* path);
bool k_file_system_is_directory(const char* path);

char** k_file_create_directory_list(const char* path, size_t* count);
char** k_file_create_absolute_directory_list(const char* path, size_t* count);

char* k_file_load(const char* path);

const char* k_file_get_working_dir();

#endif // FILE_SYSTEM_H
