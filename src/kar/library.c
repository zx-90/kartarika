
// TODO: Добавить лицензию. Скорее всего BSD-3.

#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

// ----------------------------------------------------------------------------
// Кар.Типы
// ----------------------------------------------------------------------------

const int8_t _kartarika_library_int8_min = -128;
const int8_t _kartarika_library_int8_max = 127;

const int16_t _kartarika_library_int16_min = -32768;
const int16_t _kartarika_library_int16_max = 32767;

const int32_t _kartarika_library_int32_min = -2147483648;
const int32_t _kartarika_library_int32_max = 2147483647;

const int64_t _kartarika_library_int64_min = -9223372036854775808U;
const int64_t _kartarika_library_int64_max = 9223372036854775807U;

const uint8_t _kartarika_library_uint8_max = 255;

const uint16_t _kartarika_library_uint16_max = 65535;

const uint32_t _kartarika_library_uint32_max = 4294967295;

const uint64_t _kartarika_library_uint64_max = 18446744073709551615U;

#ifndef __float32_t_defined
#   if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#       define __float32_t_defined
        typedef float float32_t TYPE_MODE(__SF__);
#   elif FLT_MANT_DIG==24
#       define __float32_t_defined
        typedef float float32_t;
#   elif DBL_MANT_DIG==24
#       define __float32_t_defined
        typedef double float32_t;
#   else
#       error TBD code for float32_t
#   endif
#endif

#ifndef __float64_t_defined
#   if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#       define __float64_t_defined
        typedef float float64_t TYPE_MODE(__DF__);
#   elif FLT_MANT_DIG==53
#       define __float64_t_defined
        typedef float float64_t;
#   elif DBL_MANT_DIG==53
#       define __float64_t_defined
        typedef double float64_t;
#   elif LDBL_MANT_DIG==53
#       define __float64_t_defined
        typedef long double float64_t;
#   else
#       error TBD code for float64_t
#   endif
#endif

const int32_t _kartarika_library_float32_bit_exponent = 7;
const int32_t _kartarika_library_float32_bit_mantissa = 24;
const int32_t _kartarika_library_float32_exponent_min = -125;
const int32_t _kartarika_library_float32_exponent_max = 128;
const float32_t _kartarika_library_float32_min = 1.17549e-38;
const float32_t _kartarika_library_float32_max = 3.40282e+38;
const float32_t _kartarika_library_float32_epsilon = 1.19209e-07;

const int32_t _kartarika_library_float64_bit_exponent = 10;
const int32_t _kartarika_library_float64_bit_mantissa = 53;
const int32_t _kartarika_library_float64_exponent_min = -1021;
const int32_t _kartarika_library_float64_exponent_max = 1024;
const float64_t _kartarika_library_float64_min = 2.22507e-308;
const float64_t _kartarika_library_float64_max = 1.79769e+308;
const float64_t _kartarika_library_float64_epsilon = 2.22045e-16;

typedef struct {
	size_t count;
	// TODO: Добавить ссылку на информацию о модуле.
	void* value;
} _kartarika_smart_pointer;

_kartarika_smart_pointer* _kartarika_smart_pointer_create(void* value) {
	_kartarika_smart_pointer* result = (_kartarika_smart_pointer*)calloc(1, sizeof(_kartarika_smart_pointer));

	result->count = 1;
	result->value = value;
	return result;
}

void _kartarika_smart_pointer_add_ref(_kartarika_smart_pointer* pointer) {
	pointer->count++;
}

void _kartarika_smart_pointer_free(_kartarika_smart_pointer* pointer) {
	pointer->count--;
	if (pointer->count == 0) {
		// TODO: Здесь надо уничтожать значение через специальную функцию, определённую для каждого класса.
		//       Для строк и неопределённостей пока сойдёт.
		free(pointer->value);
	}
}

_kartarika_smart_pointer* _kartarika_library_string_create(char* str) {
	size_t len = strlen(str);
	char* str1 = (char*)malloc(sizeof(char) * (len + 1));
	strcpy(str1, str);
	_kartarika_smart_pointer* result = _kartarika_smart_pointer_create(str1);

	return result;
}

_kartarika_smart_pointer* _kartarika_library_string_sum(_kartarika_smart_pointer* str1, _kartarika_smart_pointer* str2) {
	size_t len1 = strlen((char*)str1->value);
	size_t len2 = strlen((char*)str2->value);
	size_t len = len1 + len2 + 1;
	char* str = (char*)malloc(sizeof(char) * len);
	strcpy(str, (char*)str1->value);
	strcpy(str + len1, (char*)str2->value);
	str[len - 1] = 0;
	return _kartarika_smart_pointer_create(str);
}

static bool is_char_begin(char c) {
	return ((c & 0x80) == 0) ||
			(((c ^ 0xC0) & 0xE0) == 0) ||
			(((c ^ 0xE0) & 0xF0) == 0) ||
			(((c ^ 0xF0) & 0xF8) == 0);
}

uint32_t _kartarika_library_string_length(_kartarika_smart_pointer* str) {
	uint32_t len = 0;
	char* cur = (char*)str->value;
	while (cur) {
		if (is_char_begin(*cur)) {
			len++;
		}
		cur++;
	}
	return len;
}

uint32_t* _kartarika_library_string_find(_kartarika_smart_pointer* str, _kartarika_smart_pointer* substr) {
	size_t len = strlen((char*)str->value);
	size_t sublen = strlen((char*)substr->value);
	if (sublen > len) {
		return NULL;
	}
	uint32_t pos = 0;
	for (size_t i = 0; i < len - sublen; i++) {
		if (!is_char_begin(((char*)str->value)[i])) {
			continue;
		}
		if (memcpy((char*)str->value + i, (char*)substr->value, sublen) == 0) {
			uint32_t* result = (uint32_t*)malloc(sizeof(uint32_t));
			*result = pos;
			return result;
		}
		pos++;
	}
	return NULL;
}

uint32_t* _kartarika_library_string_find_from(_kartarika_smart_pointer* str, uint32_t pos, _kartarika_smart_pointer* substr) {
	size_t len = strlen((char*)str->value);
	size_t sublen = strlen((char*)substr->value);
	if (sublen > len) {
		return NULL;
	}
	uint32_t curpos = 0;
	for (size_t i = 0; i < len - sublen; i++) {
		if (!is_char_begin(((char*)str->value)[i])) {
			continue;
		}
		if (curpos >= pos && memcpy((char*)str->value + i, (char*)substr->value, sublen) == 0) {
			uint32_t* result = (uint32_t*)malloc(sizeof(uint32_t));
			*result = curpos;
			return result;
		}
		curpos++;
	}
	return NULL;	
}

_kartarika_smart_pointer* _kartarika_library_string_substring(_kartarika_smart_pointer* str, uint32_t pos, uint32_t length) {
	size_t bytelen = strlen((char*)str->value);
	size_t bytebegin = 0;
	size_t byteend = 0;
	bool foundbegin = false;
	bool foundend = false;
	
	uint32_t charpos = 0;
	for (size_t i = 0; i < bytelen; i++) {
		if (!is_char_begin(((char*)str->value)[i])) {
			continue;
		}
		if (charpos == pos) {
			bytebegin = i;
			foundbegin = true;
		}
		if (charpos == pos + length) {
			byteend = i;
			foundend = true;
			break;
		}
		charpos++;
	}
	if (!foundbegin) {
		char* strres = (char*)malloc(sizeof(char));
		strres[0] = 0;
		return _kartarika_library_string_create(strres);
	}
	if (!foundend) {
		byteend = bytelen;
	}
	char* strres = (char*)malloc(sizeof(char) * (byteend - bytebegin + 1));
	memcpy(strres, (char*)str->value + bytebegin, byteend - bytebegin);
	strres[byteend - bytebegin] = 0;
	return _kartarika_smart_pointer_create(strres);
}

bool _kartarika_unclean_is_empty(_kartarika_smart_pointer* value) {
	return (value == NULL || value->value == NULL);
}

bool _kartarika_unclean_bool(_kartarika_smart_pointer* value) {
	return *((bool*)value->value);
}

int8_t _kartarika_unclean_integer8(_kartarika_smart_pointer* value) {
	return *((int8_t*)value->value);
}

int16_t _kartarika_unclean_integer16(_kartarika_smart_pointer* value) {
	return *((int16_t*)value->value);
}

int32_t _kartarika_unclean_integer32(_kartarika_smart_pointer* value) {
	return *((int32_t*)value->value);
}

int64_t _kartarika_unclean_integer64(_kartarika_smart_pointer* value) {
	return *((int64_t*)value->value);
}

uint8_t _kartarika_unclean_unsigned8(_kartarika_smart_pointer* value) {
	return *((uint8_t*)value->value);
}

uint16_t _kartarika_unclean_unsigned16(_kartarika_smart_pointer* value) {
	return *((uint16_t*)value->value);
}

uint32_t _kartarika_unclean_unsigned32(_kartarika_smart_pointer* value) {
	return *((uint32_t*)value->value);
}

uint64_t _kartarika_unclean_unsigned64(_kartarika_smart_pointer* value) {
	return *((uint64_t*)value->value);
}

float32_t _kartarika_unclean_float32(_kartarika_smart_pointer* value) {
	return *((float32_t*)value->value);
}

float64_t _kartarika_unclean_float64(_kartarika_smart_pointer* value) {
	return *((float64_t*)value->value);
}

_kartarika_smart_pointer* _kartarika_unclean_string(_kartarika_smart_pointer* value) {
	return (_kartarika_smart_pointer*)value->value;
}

// ----------------------------------------------------------------------------
// Преобразование типов
// ----------------------------------------------------------------------------

bool _kartarika_library_convert_bool_to_bool(bool value) {
	return value;
}

#define _KARTARIKA_CONVERT_BOOL_TO_INTEGER(num)\
int##num##_t _kartarika_library_convert_bool_to_integer##num(bool value) { \
	if (value) { \
		return 1; \
	} \
	return 0; \
}

_KARTARIKA_CONVERT_BOOL_TO_INTEGER(8)
_KARTARIKA_CONVERT_BOOL_TO_INTEGER(16)
_KARTARIKA_CONVERT_BOOL_TO_INTEGER(32)
_KARTARIKA_CONVERT_BOOL_TO_INTEGER(64)

#define _KARTARIKA_CONVERT_BOOL_TO_UNSIGNED(num)\
uint##num##_t _kartarika_library_convert_bool_to_unsigned##num(bool value) { \
	if (value) { \
		return 1; \
	} \
	return 0; \
}

_KARTARIKA_CONVERT_BOOL_TO_UNSIGNED(8)
_KARTARIKA_CONVERT_BOOL_TO_UNSIGNED(16)
_KARTARIKA_CONVERT_BOOL_TO_UNSIGNED(32)
_KARTARIKA_CONVERT_BOOL_TO_UNSIGNED(64)

#define _KARTARIKA_CONVERT_BOOL_TO_FLOAT(num)\
float##num##_t _kartarika_library_convert_bool_to_float##num(bool value) { \
	if (value) { \
		return 1.0; \
	} \
	return 0.0; \
}

_KARTARIKA_CONVERT_BOOL_TO_FLOAT(32)
_KARTARIKA_CONVERT_BOOL_TO_FLOAT(64)

_kartarika_smart_pointer* _kartarika_library_convert_bool_to_string(bool value) {
	if (value) {
		return _kartarika_library_string_create("1");
	}
	return _kartarika_library_string_create("0");
}

#define _KARTARIKA_CONVERT_INTEGER_TO_BOOL(num)\
_kartarika_smart_pointer* _kartarika_library_convert_integer##num##_to_bool(int##num##_t value) {\
	if (value == 0) {\
		bool* b = (bool*)malloc(sizeof(bool));\
		*b = false;\
		return _kartarika_smart_pointer_create(b);\
	} else if (value == 1) {\
		bool* b = (bool*)malloc(sizeof(bool));\
		*b = true;\
		return _kartarika_smart_pointer_create(b);\
	}\
	return _kartarika_smart_pointer_create(NULL);\
}

_KARTARIKA_CONVERT_INTEGER_TO_BOOL(8)
_KARTARIKA_CONVERT_INTEGER_TO_BOOL(16)
_KARTARIKA_CONVERT_INTEGER_TO_BOOL(32)
_KARTARIKA_CONVERT_INTEGER_TO_BOOL(64)

#define _KARTARIKA_CONVERT_INTEGER_TO_INTEGER(num_from, num_to)\
int##num_to##_t _kartarika_library_convert_integer##num_from##_to_integer##num_to(int##num_from##_t value) { \
	return value; \
}

_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(8, 8)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(8, 16)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(8, 32)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(8, 64)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(16, 8)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(16, 16)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(16, 32)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(16, 64)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(32, 8)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(32, 16)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(32, 32)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(32, 64)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(64, 8)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(64, 16)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(64, 32)
_KARTARIKA_CONVERT_INTEGER_TO_INTEGER(64, 64)

#define _KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(num_from, num_to)\
uint##num_to##_t _kartarika_library_convert_integer##num_from##_to_unsigned##num_to(int##num_from##_t value) { \
	return value; \
}

_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(8, 8)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(8, 16)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(8, 32)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(8, 64)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(16, 8)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(16, 16)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(16, 32)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(16, 64)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(32, 8)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(32, 16)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(32, 32)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(32, 64)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(64, 8)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(64, 16)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(64, 32)
_KARTARIKA_CONVERT_INTEGER_TO_UNSIGNED(64, 64)

#define _KARTARIKA_CONVERT_INTEGER_TO_FLOAT(num_from, num_to)\
float##num_to##_t _kartarika_library_convert_integer##num_from##_to_float##num_to(int##num_from##_t value) { \
	return value; \
}

_KARTARIKA_CONVERT_INTEGER_TO_FLOAT(8, 32)
_KARTARIKA_CONVERT_INTEGER_TO_FLOAT(8, 64)
_KARTARIKA_CONVERT_INTEGER_TO_FLOAT(16, 32)
_KARTARIKA_CONVERT_INTEGER_TO_FLOAT(16, 64)
_KARTARIKA_CONVERT_INTEGER_TO_FLOAT(32, 32)
_KARTARIKA_CONVERT_INTEGER_TO_FLOAT(32, 64)
_KARTARIKA_CONVERT_INTEGER_TO_FLOAT(64, 32)
_KARTARIKA_CONVERT_INTEGER_TO_FLOAT(64, 64)

#define _KARTARIKA_CONVERT_INTEGER_TO_STRING(num)\
_kartarika_smart_pointer* _kartarika_library_convert_integer##num##_to_string(int##num##_t value) {\
	char str[21];\
	sprintf(str,"%lld", (long long int)value);\
	return _kartarika_library_string_create(str);\
}

_KARTARIKA_CONVERT_INTEGER_TO_STRING(8)
_KARTARIKA_CONVERT_INTEGER_TO_STRING(16)
_KARTARIKA_CONVERT_INTEGER_TO_STRING(32)
_KARTARIKA_CONVERT_INTEGER_TO_STRING(64)

#define _KARTARIKA_CONVERT_UNSIGNED_TO_BOOL(num)\
_kartarika_smart_pointer* _kartarika_library_convert_unsigned##num##_to_bool(uint##num##_t value) {\
	if (value == 0) {\
		bool* b = (bool*)malloc(sizeof(bool));\
		*b = false;\
		return _kartarika_smart_pointer_create(b);\
	} else if (value == 1) {\
		bool* b = (bool*)malloc(sizeof(bool));\
		*b = true;\
		return _kartarika_smart_pointer_create(b);\
	}\
	return _kartarika_smart_pointer_create(NULL);\
}

_KARTARIKA_CONVERT_UNSIGNED_TO_BOOL(8)
_KARTARIKA_CONVERT_UNSIGNED_TO_BOOL(16)
_KARTARIKA_CONVERT_UNSIGNED_TO_BOOL(32)
_KARTARIKA_CONVERT_UNSIGNED_TO_BOOL(64)

#define _KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(num_from, num_to)\
int##num_to##_t _kartarika_library_convert_unsigned##num_from##_to_integer##num_to(uint##num_from##_t value) { \
	return value; \
}

_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(8, 8)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(8, 16)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(8, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(8, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(16, 8)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(16, 16)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(16, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(16, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(32, 8)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(32, 16)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(32, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(32, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(64, 8)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(64, 16)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(64, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_INTEGER(64, 64)

#define _KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(num_from, num_to)\
uint##num_to##_t _kartarika_library_convert_unsigned##num_from##_to_unsigned##num_to(uint##num_from##_t value) { \
	return value; \
}

_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(8, 8)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(8, 16)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(8, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(8, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(16, 8)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(16, 16)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(16, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(16, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(32, 8)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(32, 16)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(32, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(32, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(64, 8)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(64, 16)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(64, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_UNSIGNED(64, 64)

#define _KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(num_from, num_to)\
float##num_to##_t _kartarika_library_convert_unsigned##num_from##_to_float##num_to(uint##num_from##_t value) { \
	return value; \
}

_KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(8, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(8, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(16, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(16, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(32, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(32, 64)
_KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(64, 32)
_KARTARIKA_CONVERT_UNSIGNED_TO_FLOAT(64, 64)

#define _KARTARIKA_CONVERT_UNSIGNED_TO_STRING(num)\
_kartarika_smart_pointer* _kartarika_library_convert_unsigned##num##_to_string(uint##num##_t value) {\
	char str[21];\
	sprintf(str,"%llu", (long long unsigned int)value);\
	return _kartarika_library_string_create(str);\
}

_KARTARIKA_CONVERT_UNSIGNED_TO_STRING(8)
_KARTARIKA_CONVERT_UNSIGNED_TO_STRING(16)
_KARTARIKA_CONVERT_UNSIGNED_TO_STRING(32)
_KARTARIKA_CONVERT_UNSIGNED_TO_STRING(64)

#define _KARTARIKA_CONVERT_FLOAT_TO_BOOL(num)\
_kartarika_smart_pointer* _kartarika_library_convert_float##num##_to_bool(float##num##_t value) {\
	if (value == 0.0) {\
		bool* b = (bool*)malloc(sizeof(bool));\
		*b = false;\
		return _kartarika_smart_pointer_create(b);\
	} else if (value == 1.0) {\
		bool* b = (bool*)malloc(sizeof(bool));\
		*b = true;\
		return _kartarika_smart_pointer_create(b);\
	}\
	return _kartarika_smart_pointer_create(NULL);\
}

_KARTARIKA_CONVERT_FLOAT_TO_BOOL(32)
_KARTARIKA_CONVERT_FLOAT_TO_BOOL(64)

#define _KARTARIKA_CONVERT_FLOAT_TO_FLOAT(num1, num2)\
float##num2##_t _kartarika_library_convert_float##num1##_to_float##num2(float##num1##_t value) {\
	return (float##num2##_t)value;\
}

_KARTARIKA_CONVERT_FLOAT_TO_FLOAT(32, 32)
_KARTARIKA_CONVERT_FLOAT_TO_FLOAT(32, 64)
_KARTARIKA_CONVERT_FLOAT_TO_FLOAT(64, 32)
_KARTARIKA_CONVERT_FLOAT_TO_FLOAT(64, 64)

_kartarika_smart_pointer* _kartarika_library_convert_string_to_bool(_kartarika_smart_pointer* str) {
	char* val = (char*)str->value;
	if (
			!strcmp(val, "да") ||
			!strcmp(val, "Да") ||
			!strcmp(val, "дА") ||
			!strcmp(val, "ДА")) {
		// true
		bool* b = (bool*)malloc(sizeof(bool));
		*b = true;
		return _kartarika_smart_pointer_create(b);
	} else if (
			!strcmp(val, "нет") ||
			!strcmp(val, "Нет") ||
			!strcmp(val, "нЕт") ||
			!strcmp(val, "НЕт") ||
			!strcmp(val, "неТ") ||
			!strcmp(val, "НеТ") ||
			!strcmp(val, "нЕТ") ||
			!strcmp(val, "НЕТ")) {
		// false
		bool* b = (bool*)malloc(sizeof(bool));
		*b = false;
		return _kartarika_smart_pointer_create(b);
	}
	return _kartarika_smart_pointer_create(NULL);
}

#define _KARTARIKA_CONVERT_STRING_TO_INTEGER(num)\
_kartarika_smart_pointer* _kartarika_library_convert_string_to_integer##num(_kartarika_smart_pointer* str) {\
	char* end;\
	long long int conv = strtoll(str->value, &end, 10);\
	if (*end != 0) {\
		return _kartarika_smart_pointer_create(NULL);\
	}\
	if (conv > INT##num##_MAX || conv < INT##num##_MIN) {\
		return _kartarika_smart_pointer_create(NULL);\
	}\
	int##num##_t* res = (int##num##_t*)malloc(sizeof(int##num##_t));\
	*res = conv;\
	return _kartarika_smart_pointer_create(res);\
}

_KARTARIKA_CONVERT_STRING_TO_INTEGER(8)
_KARTARIKA_CONVERT_STRING_TO_INTEGER(16)
_KARTARIKA_CONVERT_STRING_TO_INTEGER(32)
_KARTARIKA_CONVERT_STRING_TO_INTEGER(64)

#define _KARTARIKA_CONVERT_STRING_TO_UNSIGNED(num)\
_kartarika_smart_pointer* _kartarika_library_convert_string_to_unsigned##num(_kartarika_smart_pointer* str) {\
	char* end;\
	long long unsigned conv = strtoull(str->value, &end, 10);\
	if (*end != 0) {\
		return _kartarika_smart_pointer_create(NULL);\
	}\
	if (conv > UINT##num##_MAX) {\
		return _kartarika_smart_pointer_create(NULL);\
	}\
	uint##num##_t* res = (uint##num##_t*)malloc(sizeof(uint##num##_t));\
	*res = conv;\
	return _kartarika_smart_pointer_create(res);\
}

_KARTARIKA_CONVERT_STRING_TO_UNSIGNED(8)
_KARTARIKA_CONVERT_STRING_TO_UNSIGNED(16)
_KARTARIKA_CONVERT_STRING_TO_UNSIGNED(32)
_KARTARIKA_CONVERT_STRING_TO_UNSIGNED(64)

// TODO: Реализовать. Пока просто заглушка.
#define _KARTARIKA_CONVERT_STRING_TO_FLOAT(num)\
float##num##_t _kartarika_library_convert_string_to_float##num(_kartarika_smart_pointer* str) {\
	return num;\
}

_KARTARIKA_CONVERT_STRING_TO_FLOAT(32)
_KARTARIKA_CONVERT_STRING_TO_FLOAT(64)

_kartarika_smart_pointer* _kartarika_library_convert_string_to_string(_kartarika_smart_pointer* str) {
	str->count++;
	return str;
}

// ----------------------------------------------------------------------------
// Кар.Консоль
// ----------------------------------------------------------------------------

// метод Читать() Строка?
// TODO: Для линукс и POSIX систем есть getline()
static char* get_line() {
	char* line = malloc(100);
	char* linep = line;
	size_t lenmax = 100;
	size_t len = lenmax;
	int c;

	if (line == NULL) {
		return NULL;
	}

	for(;;) {
		c = fgetc(stdin);
		if (c == EOF) {
			break;
		}

		if (--len == 0) {
			len = lenmax;
			lenmax *= 2;
			char* linen = realloc(linep, lenmax);
			if (linen == NULL) {
				free(linep);
				return NULL;
			}
			line = linen + (line - linep);
			linep = linen;
		}
		if ((*line++ = c) == '\n') {
			break;
		}
	}
	*line = '\0';
	return linep;
}

_kartarika_smart_pointer* _kartarika_library_read_string() {
	char* str = get_line();
	if (str == NULL) {
		return NULL;
	}
	return _kartarika_library_string_create(str);
}

// метод Писать(Буль значение)
void _kartarika_library_write_bool(bool value) {
	if (value) {
		printf("Да");
	} else {
		printf("Нет");
	}
}

// метод Писать(Целое8 значение)
void _kartarika_library_write_int8(int8_t value) {
	printf("%"PRIi8, value);
}

// метод Писать(Целое16 значение)
void _kartarika_library_write_int16(int16_t value) {
	printf("%"PRIi16, value);
}

// метод Писать(Целое32 значение)
void _kartarika_library_write_int32(int32_t value) {
	printf("%"PRIi32, value);
}

// метод Писать(Целое64 значение)
void _kartarika_library_write_int64(int64_t value) {
	printf("%"PRIi64, value);
}

// метод Писать(Счётное8 значение)
void _kartarika_library_write_uint8(uint8_t value) {
	printf("%"PRIu8, value);
}

// метод Писать(Счётное16 значение)
void _kartarika_library_write_uint16(uint16_t value) {
	printf("%"PRIu16, value);
}

// метод Писать(Счётное32 значение)
void _kartarika_library_write_uint32(uint32_t value) {
	printf("%"PRIu32, value);
}

// метод Писать(Счётное64 значение)
void _kartarika_library_write_uint64(uint64_t value) {
	printf("%"PRIu64, value);
}

// метод Писать(Дробное32 значение)
void _kartarika_library_write_float32(float32_t value) {
	// TODO: Здесь печатается float, а это не всегда 32-битное плавающее.
	float absd = fabsf(value);
	// TODO: Доделать все особые случаи.
	if (isnan(value)) {
		printf("НеЧисло");
	} else if (isinf(value) && value > 0.0) {
		printf("∞");
	} else if (isinf(value) && value < 0.0) {
		printf("-∞");
	} else if (absd >= 0.0001 && absd < 10000.0 || absd == 0.0) {
		char output[50];
		snprintf(output, 50, "%f", value);
		char* cur = output;
		while (*cur != 0) {
			if (*cur == '.') {
				*cur = ',';
			}
			cur++;
		}
		printf("%s", output);
	} else {
		char output[50];
		snprintf(output, 50, "%e", value);
		char* cur = output;
		while (*cur != 0) {
			if (*cur == '.') {
				*cur = ',';
			}
			cur++;
		}
		while (cur != output) {
			cur[1] = cur[0];
			// Меняем e на с
			if (cur[0] == 'e') {
				memcpy(cur, "с", 2);
				break;
			}
			cur--;
		}
		printf("%s", output);
	}
}

// метод Писать(Дробное64 значение)
void _kartarika_library_write_float64(float64_t value) {
	// TODO: Здесь печатается double, а это не всегда 64-битное плавающее.
	double absd = fabs(value);
	// TODO: Доделать все особые случаи.
	if (isnan(value)) {
		printf("НеЧисло");
	} else if (isinf(value) && value > 0.0) {
		printf("∞");
	} else if (isinf(value) && value < 0.0) {
		printf("-∞");
	} else if (absd >= 0.0001 && absd < 10000.0 || absd == 0.0) {
		char output[50];
		snprintf(output, 50, "%f", value);
		char* cur = output;
		while (*cur != 0) {
			if (*cur == '.') {
				*cur = ',';
			}
			cur++;
		}
		printf("%s", output);
	} else {
		char output[50];
		snprintf(output, 50, "%e", value);
		char* cur = output;
		while (*cur != 0) {
			if (*cur == '.') {
				*cur = ',';
			}
			cur++;
		}
		while (cur != output) {
			cur[1] = cur[0];
			// Меняем e на с
			if (cur[0] == 'e') {
				memcpy(cur, "с", 2);
				break;
			}
			cur--;
		}
		printf("%s", output);
	}
}

// метод Писать(Строка значение)
void _kartarika_library_write_string(_kartarika_smart_pointer* value) {
	printf("%s", (char*)value->value);
}

void _kartarika_library_write_chars(char* value) {
	printf("%s", value);
}

// ----------------------------------------------------------------------------
// Кар.ПотокОшибок
// ----------------------------------------------------------------------------
// метод Писать(Буль значение)
void _kartarika_library_error_write_bool(bool value) {
	if (value) {
		fprintf(stderr, "Да");
	} else {
		fprintf(stderr, "Нет");
	}
}

// метод Писать(Целое8 значение)
void _kartarika_library_error_write_int8(int8_t value) {
	fprintf(stderr, "%"PRIi8, value);
}

// метод Писать(Целое16 значение)
void _kartarika_library_error_write_int16(int16_t value) {
	fprintf(stderr, "%"PRIi16, value);
}

// метод Писать(Целое32 значение)
void _kartarika_library_error_write_int32(int32_t value) {
	fprintf(stderr, "%"PRIi32, value);
}

// метод Писать(Целое64 значение)
void _kartarika_library_error_write_int64(int64_t value) {
	fprintf(stderr, "%"PRIi64, value);
}

// метод Писать(Счётное8 значение)
void _kartarika_library_error_write_uint8(uint8_t value) {
	fprintf(stderr, "%"PRIu8, value);
}

// метод Писать(Счётное16 значение)
void _kartarika_library_error_write_uint16(uint16_t value) {
	fprintf(stderr, "%"PRIu16, value);
}

// метод Писать(Счётное32 значение)
void _kartarika_library_error_write_uint32(uint32_t value) {
	fprintf(stderr, "%"PRIu32, value);
}

// метод Писать(Счётное64 значение)
void _kartarika_library_error_write_uint64(uint64_t value) {
	fprintf(stderr, "%"PRIu64, value);
}

// метод Писать(Дробное32 значение)
void _kartarika_library_error_write_float32(float32_t value) {
	// TODO: Здесь печатается float, а это не всегда 32-битное плавающее.
	fprintf(stderr, "%f", value);
}

// метод Писать(Дробное64 значение)
void _kartarika_library_error_write_float64(float64_t value) {
	// TODO: Здесь печатается double, а это не всегда 64-битное плавающее.
	fprintf(stderr, "%lf", value);
}

// метод Писать(Строка значение)
void _kartarika_library_error_write_string(_kartarika_smart_pointer* value) {
	fprintf(stderr, "%s", (char*)value->value);
}

void _kartarika_library_error_write_chars(char* value) {
	fprintf(stderr, "%s", value);
}


// ----------------------------------------------------------------------------
// Кар.Мат
// ----------------------------------------------------------------------------

// стат метод ВзятьПСЧ(Счётное значение) Счётное
uint32_t _kartarika_library_get_prn(uint32_t prev) {
	return prev + (prev << 7) + (prev << 16) + 12345;
}

// стат метод ВзятьСлучайное() Счётное
uint32_t _kartarika_library_get_random() {
	// TODO: Сделать.
	return 0;
}
