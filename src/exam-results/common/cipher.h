#ifndef CIPHER_H
#define CIPHER_H

#include <stddef.h>

/*
 * Шифрование паролей шифром Виженера.
 *
 * Алфавит — латинские буквы и цифры (62 символа). Каждый символ пароля
 * сдвигается по алфавиту на номер соответствующего символа ключа;
 * ключ повторяется, если пароль длиннее ключа.
 *
 * В базе данных хранится зашифрованный пароль (поле users.password_hash).
 */

/* 1, если строка состоит только из латинских букв и цифр. */
int is_cipher_alphabet(const char *text);

/*
 * Шифрует / расшифровывает строку в буфер out размера size.
 * Возвращает 1 — успешно, 0 — в строке есть символы не из алфавита
 * или строка не помещается в буфер.
 */
int encrypt_password(const char *plain, char *out, size_t size);
int decrypt_password(const char *cipher, char *out, size_t size);

#endif /* CIPHER_H */
