/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/project.h"

#include "core/alloc.h"

KarProject* kar_project_create(char* filename) {
	KAR_CREATE(project, KarProject);
	
	project->module = kar_module_create(filename);
	project->vartree = kar_vartree_create();
	kar_array_init(&project->def_list);
	
	return project;
}

void kar_project_free(KarProject* project) {
	kar_module_free(project->module);
	kar_vartree_free(project->vartree);
	kar_array_clear(&project->def_list, NULL);
	KAR_FREE(project);
}

KAR_TREE_CODE(project_def_list, KarProject, KarVartree, def_list, &kar_vartree_free)
