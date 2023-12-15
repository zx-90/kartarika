/* Copyright © 2020,2021,2023 Evgeny Zaytsev <zx_90@mail.ru>
 * Copyright © 2021 Abdullin Timur <abdtimurrif@gmail.com>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_FILE_SYSTEM_H
#define KAR_FILE_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "string_list.h"

extern const KarString* KAR_FILE_SYSTEM_DELIMETER;

bool kar_file_system_is_file(const KarString* path);
bool kar_file_system_is_directory(const KarString* path);

KarString* kar_file_system_get_basename(KarString* path);

KarStringList* kar_file_create_absolute_directory_list(const KarString* path);

FILE* kar_file_system_create_handle(const KarString* path);

KarString* kar_file_load(const KarString* path);

const KarString* kar_file_get_working_dir();

#endif // KAR_FILE_SYSTEM_H
