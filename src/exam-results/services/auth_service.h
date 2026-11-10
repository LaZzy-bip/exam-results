#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "../models/user.h"

/*
 * Проверяет логин и пароль. Введённый пароль шифруется тем же шифром
 * и сравнивается с зашифрованным паролем из базы.
 * При успехе заполняет out и возвращает SERVICE_OK.
 */
int authenticate(const char *login, const char *password, User *out, char *err);

#endif
