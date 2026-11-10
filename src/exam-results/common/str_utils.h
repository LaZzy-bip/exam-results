#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <stddef.h>

void str_copy(char *dst, size_t size, const char *src);

char *str_trim(char *s);

int str_is_blank(const char *s);

int str_has_space(const char *s);

size_t utf8_length(const char *s);

int utf8_equal_ci(const char *a, const char *b);

#endif
