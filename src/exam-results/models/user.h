#ifndef MODEL_USER_H
#define MODEL_USER_H

#include "sizes.h"

/* Таблица users */
typedef struct {
    int  user_id;
    char login[LOGIN_LEN];
    char password_hash[PASSWORD_LEN];   /* пароль, зашифрованный шифром Виженера */
    char full_name[FULL_NAME_LEN];
    int  user_role;

    /* служебное поле: название роли из user_roles (в таблице users его нет) */
    char role_name[ROLE_NAME_LEN];
} User;

#endif /* MODEL_USER_H */
