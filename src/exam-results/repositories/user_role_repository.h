#ifndef USER_ROLE_REPOSITORY_H
#define USER_ROLE_REPOSITORY_H

#include "../models/user_role.h"

/* Роли — справочник, заполняется в schema.sql, поэтому только чтение. */
int user_role_repository_get_by_id(int user_role_id, UserRole *out);
int user_role_repository_get_all(UserRole **out);

#endif
