/* Copyright © 2020 Evgeny Zaytsev <zx_90@mail.ru>
 * 
 * Distributed under the terms of the GNU LGPL v3 license. See accompanying
 * file LICENSE or copy at https://www.gnu.org/licenses/lgpl-3.0.html
*/

#include "analyzer/analyzer.h"

#include "model/token.h"

/*static bool fill_standard_lib(KarProject* project) {
	KarVartree* vartree = project->vartree;
	KarVartree** def_list = project->def_list;
	
	
	return true;
}*/

bool kar_analyzer_run(KarProject* project) {
	if (!project) {
		return false;
	}
	return true;
}
