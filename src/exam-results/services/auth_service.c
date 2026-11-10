#include "auth_service.h"
#include "service_error.h"
#include "../common/cipher.h"
#include "../common/str_utils.h"
#include "../repositories/user_repository.h"

#include <string.h>

int authenticate(const char *login, const char *password, User *out, char *err)
{
    if (str_is_blank(login) || str_is_blank(password)) {
        return service_fail(err, "Введите логин и пароль");
    }

    User user;
    int found = user_repository_get_by_login(login, &user);
    if (found < 0) {
        return service_fail(err, "Ошибка базы данных");
    }

    /* пароль с недопустимыми символами зашифровать нельзя — значит, он точно неверный */
    char encrypted[PASSWORD_LEN];
    int ok = encrypt_password(password, encrypted, sizeof encrypted);

    /* одинаковое сообщение, чтобы нельзя было подобрать существующий логин */
    if (found == 0 || !ok || strcmp(user.password_hash, encrypted) != 0) {
        return service_fail(err, "Неверный логин или пароль");
    }

    *out = user;
    return SERVICE_OK;
}
