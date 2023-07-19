
// TODO: Добавить лицензию. Скорее всего BSD-3.

#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

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
	size_t refs;
	char* str;
} _kartarika_library_string;

_kartarika_library_string* _kartarika_library_string_create(char* str) {
	_kartarika_library_string* result = (_kartarika_library_string*)malloc(sizeof(_kartarika_library_string));

	result->refs = 1;
	result->str = str;

	return result;
}

void _kartarika_library_string_add_ref(_kartarika_library_string* str) {
	str->refs++;
}

void _kartarika_library_string_free(_kartarika_library_string* str) {
	str->refs--;
	if (str->refs == 0) {
		free(str->str);
		free(str);
	}
}

_kartarika_library_string* _kartarika_library_string_sum(_kartarika_library_string* str1, _kartarika_library_string* str2) {
	size_t len1 = strlen(str1->str);
	size_t len2 = strlen(str2->str);
	size_t len = len1 + len2 + 1;
	char* str = (char*)malloc(sizeof(char) * len);
	strcpy(str, str1->str);
	strcpy(str + len1, str2->str);
	str[len - 1] = 0;
	return _kartarika_library_string_create(str);
}

static bool is_char_begin(char c) {
	return ((c & 0x80) == 0) ||
			(((c ^ 0xC0) & 0xE0) == 0) ||
			(((c ^ 0xE0) & 0xF0) == 0) ||
			(((c ^ 0xF0) & 0xF8) == 0);
}

uint32_t _kartarika_library_string_length(_kartarika_library_string* str) {
	uint32_t len = 0;
	char* cur = str->str;
	while (cur) {
		if (is_char_begin(*cur)) {
			len++;
		}
		cur++;
	}
	return len;
}

uint32_t* _kartarika_library_string_find(_kartarika_library_string* str, _kartarika_library_string* substr) {
	size_t len = strlen(str->str);
	size_t sublen = strlen(substr->str);
	if (sublen > len) {
		return NULL;
	}
	uint32_t pos = 0;
	for (size_t i = 0; i < len - sublen; i++) {
		if (!is_char_begin(str->str[i])) {
			continue;
		}
		if (memcpy(str->str + i, substr->str, sublen) == 0) {
			uint32_t* result = (uint32_t*)malloc(sizeof(uint32_t));
			*result = pos;
			return result;
		}
		pos++;
	}
	return NULL;
}

uint32_t* _kartarika_library_string_find_from(_kartarika_library_string* str, uint32_t pos, _kartarika_library_string* substr) {
	size_t len = strlen(str->str);
	size_t sublen = strlen(substr->str);
	if (sublen > len) {
		return NULL;
	}
	uint32_t curpos = 0;
	for (size_t i = 0; i < len - sublen; i++) {
		if (!is_char_begin(str->str[i])) {
			continue;
		}
		if (curpos >= pos && memcpy(str->str + i, substr->str, sublen) == 0) {
			uint32_t* result = (uint32_t*)malloc(sizeof(uint32_t));
			*result = curpos;
			return result;
		}
		curpos++;
	}
	return NULL;	
}

_kartarika_library_string* _kartarika_library_string_substring(_kartarika_library_string* str, uint32_t pos, uint32_t length) {
	size_t bytelen = strlen(str->str);
	size_t bytebegin = 0;
	size_t byteend = 0;
	bool foundbegin = false;
	bool foundend = false;
	
	uint32_t charpos = 0;
	for (size_t i = 0; i < bytelen; i++) {
		if (!is_char_begin(str->str[i])) {
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
	memcpy(strres, str->str + bytebegin, byteend - bytebegin);
	strres[byteend - bytebegin] = 0;
	return _kartarika_library_string_create(strres);
}

// ----------------------------------------------------------------------------
// Преобразование типов
// ----------------------------------------------------------------------------



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

_kartarika_library_string* _kartarika_library_read_string() {
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
	printf("%f", value);
}

// метод Писать(Дробное64 значение)
void _kartarika_library_write_float64(float64_t value) {
	// TODO: Здесь печатается double, а это не всегда 64-битное плавающее.
	printf("%lf", value);
}

// метод Писать(Строка значение)
void _kartarika_library_write_string(_kartarika_library_string* value) {
	printf("%s", value->str);
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
void _kartarika_library_error_write_string(_kartarika_library_string* value) {
	fprintf(stderr, "%s", value->str);
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
