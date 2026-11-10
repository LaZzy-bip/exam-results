#ifndef USER_ROLE_REPOSITORY_H
#define USER_ROLE_REPOSITORY_H

#include "../models/user_role.h"

int user_role_repository_get_by_id(int user_role_id, UserRole *out);
int user_role_repository_get_all(UserRole **out);

#endif
