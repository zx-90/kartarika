
#include "model/vartree_var.h"

KarVartree* kar_vartree_create_variable(KarVartree *parent, const KarString* name, KarVartree* type) {
	if (parent == NULL ||
		(parent->type != KAR_VARTYPE_CLASS && parent->type != KAR_VARTYPE_UNCLEAN && parent->type != KAR_VARTYPE_UNCLEAN_CLASS)
	) {
		return NULL;
	}
	if (kar_vartree_find(parent, name) != NULL) {
		return NULL;
	}
	KarVartree* result = kar_vartree_create_name(KAR_VARTYPE_VARIABLE, name);
	result->params = type;
	kar_vartree_child_add(parent, result);
	return result;
}

//-----------------------------------------------------------------------------
// Описание констант.
//-----------------------------------------------------------------------------

// TODO: Для разных типов констант необходимы разные структуры, а не просто (void* value).
KarVartreeConstValue* kar_vartree_const_value_create(uint8_t modificators, KarVartree* type, void* const_value) {
	KAR_CREATE(value, KarVartreeConstValue);

	value->modificators = modificators;
	value->type = type;
	value->value = const_value;

	return value;
}

void kar_vartree_const_value_free(void* ptr) {
	KarVartreeConstValue* value = (KarVartreeConstValue*)ptr;
	KAR_FREE(value);
}

KarVartree* kar_vartree_create_const(KarVartree *parent, const KarString* name, uint8_t modificators, KarVartree* type, void* value) {
	if (parent == NULL ||
		(parent->type != KAR_VARTYPE_CLASS && parent->type != KAR_VARTYPE_UNCLEAN && parent->type != KAR_VARTYPE_UNCLEAN_CLASS)
	) {
		return NULL;
	}
	if (kar_vartree_find(parent, name) != NULL) {
		return NULL;
	}
	KarVartree* result = kar_vartree_create_name(KAR_VARTYPE_CONST, name);
	result->params = kar_vartree_const_value_create(modificators, type, value);
	result->freeParams = &kar_vartree_const_value_free;
	kar_vartree_child_add(parent, result);
	return result;
}

//-----------------------------------------------------------------------------
// Описание констант закончено.
//-----------------------------------------------------------------------------


