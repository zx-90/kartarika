#ifndef KAR_VARTREE_VAR_PARAMS_H
#define KAR_VARTREE_VAR_PARAMS_H

#include "core/string.h"

typedef enum {
	VAR_STATIC = 0,
	VAR_DYNAMIC = 1,

	VAR_PRIVATE = 0 << 1,
	VAR_PROTECTED = 1 << 1,
	VAR_PUBLIC = 2 << 1,

} KarVariableModificator;

#endif // KAR_VARTREE_VAR_PARAMS_H
