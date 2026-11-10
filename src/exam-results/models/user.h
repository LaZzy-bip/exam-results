#ifndef MODEL_USER_H
#define MODEL_USER_H

#include "sizes.h"

typedef struct {
    int  user_id;
    char login[LOGIN_LEN];
    char password_hash[PASSWORD_LEN];
    char full_name[FULL_NAME_LEN];
    int  user_role;

    char role_name[ROLE_NAME_LEN];
} User;

#endif
