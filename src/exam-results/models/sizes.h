#ifndef MODELS_SIZES_H
#define MODELS_SIZES_H

/*
 * Размеры строковых полей моделей (в байтах, с учётом '\0').
 * Строки хранятся в UTF-8: одна русская буква занимает 2 байта.
 */
#define LOGIN_LEN       64
#define PASSWORD_LEN    33   /* зашифрованный пароль: до 32 символов + '\0' */
#define FULL_NAME_LEN   192
#define ROLE_NAME_LEN   64
#define GROUP_NAME_LEN  64
#define SUBJECT_LEN     128
#define DEPARTMENT_LEN  192
#define RECORD_BOOK_LEN 32
#define DATE_LEN        11   /* ГГГГ-ММ-ДД + '\0' */

#endif /* MODELS_SIZES_H */
