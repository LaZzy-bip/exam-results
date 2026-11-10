#include "cipher.h"

#include <string.h>

static const char ALPHABET[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789";

#define ALPHABET_SIZE 62

static const char KEY[] = "ExamKey";

static int index_of(char c)
{
    if (c == '\0') {
        return -1;
    }
    const char *p = strchr(ALPHABET, c);
    return p ? (int)(p - ALPHABET) : -1;
}

int is_cipher_alphabet(const char *text)
{
    for (; *text; text++) {
        if (index_of(*text) < 0) {
            return 0;
        }
    }
    return 1;
}

static int transform(const char *text, char *out, size_t size, int direction)
{
    size_t len = strlen(text);
    size_t key_len = strlen(KEY);

    if (len + 1 > size) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        int c = index_of(text[i]);
        if (c < 0) {
            return 0;
        }
        int shift = index_of(KEY[i % key_len]);
        int moved = (c + direction * shift + ALPHABET_SIZE) % ALPHABET_SIZE;
        out[i] = ALPHABET[moved];
    }
    out[len] = '\0';
    return 1;
}

int encrypt_password(const char *plain, char *out, size_t size)
{
    return transform(plain, out, size, +1);
}

int decrypt_password(const char *cipher, char *out, size_t size)
{
    return transform(cipher, out, size, -1);
}
