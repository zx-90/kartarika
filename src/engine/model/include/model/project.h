/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#ifndef KAR_PROJECT_H
#define KAR_PROJECT_H

#include "core/array.h"
#include "module.h"
#include "vartree.h"

typedef struct {
	KarModule* module;
	KarVartree* vartree;
	KarArray def_list;
} KarProject;

KarProject* kar_project_create(char* filename);
void kar_project_free(KarProject* project);

KAR_TREE_HEADER(project_def_list, KarProject, KarVartree)

#endif // KAR_PROJECT_H
