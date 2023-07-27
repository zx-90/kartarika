/* Copyright © 2023 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "model/project.h"

#include "core/alloc.h"

KarProject* kar_project_create(KarString* filename) {
	KAR_CREATE(project, KarProject);
	
	project->module = kar_module_create(filename);
    project->vars = kar_vars_create();
    project->errors = kar_project_error_list_create();

	return project;
}

void kar_project_free(KarProject* project) {
	kar_module_free(project->module);
    kar_vars_free(project->vars);
    kar_project_error_list_free(project->errors);
    KAR_FREE(project);
}
