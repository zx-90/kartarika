/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include "tester/test.h"

KError* k_test_suite_run(const char* path);

#endif // TEST_SUITE_H
