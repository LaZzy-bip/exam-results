#include "str_utils.h"

#include <ctype.h>
#include <string.h>

void str_copy(char *dst, size_t size, const char *src)
{
    if (size == 0) {
        return;
    }
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }
    size_t n = strlen(src);
    if (n >= size) {
        n = size - 1;
        while (n > 0 && ((unsigned char)src[n] & 0xC0) == 0x80) {
            n--;
        }
    }
    memcpy(dst, src, n);
    dst[n] = '\0';
}

char *str_trim(char *s)
{
    char *start = s;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) {
        s[--n] = '\0';
    }
    return s;
}

int str_is_blank(const char *s)
{
    if (s == NULL) {
        return 1;
    }
    for (; *s; s++) {
        if (!isspace((unsigned char)*s)) {
            return 0;
        }
    }
    return 1;
}

int str_has_space(const char *s)
{
    for (; *s; s++) {
        if (isspace((unsigned char)*s)) {
            return 1;
        }
    }
    return 0;
}

size_t utf8_length(const char *s)
{
    size_t len = 0;
    for (; *s; s++) {
        if (((unsigned char)*s & 0xC0) != 0x80) {
            len++;
        }
    }
    return len;
}

static unsigned long utf8_next(const char **s)
{
    const unsigned char *p = (const unsigned char *)*s;
    unsigned long cp;
    int extra;

    if (p[0] < 0x80)      { cp = p[0];        extra = 0; }
    else if (p[0] < 0xE0) { cp = p[0] & 0x1F; extra = 1; }
    else if (p[0] < 0xF0) { cp = p[0] & 0x0F; extra = 2; }
    else                  { cp = p[0] & 0x07; extra = 3; }

    p++;
    for (int i = 0; i < extra && (*p & 0xC0) == 0x80; i++, p++) {
        cp = (cp << 6) | (*p & 0x3F);
    }
    *s = (const char *)p;
    return cp;
}

static unsigned long to_lower_cp(unsigned long cp)
{
    if (cp >= 'A' && cp <= 'Z') {
        return cp + 32;
    }
    if (cp >= 0x0410 && cp <= 0x042F) {
        return cp + 0x20;
    }
    if (cp == 0x0401) {
        return 0x0451;
    }
    return cp;
}

int utf8_equal_ci(const char *a, const char *b)
{
    while (*a && *b) {
        if (to_lower_cp(utf8_next(&a)) != to_lower_cp(utf8_next(&b))) {
            return 0;
        }
    }
    return *a == '\0' && *b == '\0';
}
