#ifndef KAR_VARTREE_VAR_PARAMS_H
#define KAR_VARTREE_VAR_PARAMS_H

#include "core/string.h"
#include "model/vartree.h"

typedef enum {
	VAR_STATIC = 0,
	VAR_DYNAMIC = 1,

	VAR_PRIVATE = 0 << 1,
	VAR_PROTECTED = 1 << 1,
	VAR_PUBLIC = 2 << 1,

} KarVariableModificator;

typedef struct {
	uint8_t modificators;
	KarVartree* type;
	void* value;
} KarVartreeConstValue;

// TODO: обавить модификаторы
KarVartree* kar_vartree_create_variable(KarVartree* parent, const KarString* name, uint8_t modificators, KarVartree* type, void* value);
// TODO: Сделать функцию безопасной для поля void. Скорее надо будет разбить на несколько функций.

KarVartree* kar_vartree_create_const(KarVartree* parent, const KarString* name, uint8_t modificators, KarVartree* type, void* value);

#endif // KAR_VARTREE_VAR_PARAMS_H
