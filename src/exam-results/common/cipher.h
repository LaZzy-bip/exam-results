#ifndef CIPHER_H
#define CIPHER_H

#include <stddef.h>

int is_cipher_alphabet(const char *text);

int encrypt_password(const char *plain, char *out, size_t size);
int decrypt_password(const char *cipher, char *out, size_t size);

#endif
