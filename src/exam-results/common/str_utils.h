#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <stddef.h>

/* Безопасное копирование строки: результат всегда завершается '\0'. src == NULL -> "" */
void str_copy(char *dst, size_t size, const char *src);

/* Удаляет пробелы в начале и в конце строки (аналог strip() в Python). */
char *str_trim(char *s);

/* 1, если строка пустая или состоит только из пробелов. */
int str_is_blank(const char *s);

/* 1, если в строке есть пробельные символы. */
int str_has_space(const char *s);

/* Длина строки UTF-8 в символах (а не в байтах). */
size_t utf8_length(const char *s);

/*
 * Сравнение строк UTF-8 без учёта регистра (латиница и кириллица, включая Ё).
 * Аналог a.lower() == b.lower() в Python. Возвращает 1, если строки равны.
 */
int utf8_equal_ci(const char *a, const char *b);

#endif /* STR_UTILS_H */
