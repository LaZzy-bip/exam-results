#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "../models/user.h"

int user_repository_create(const char *login, const char *password_hash,
                           const char *full_name, int user_role);
int user_repository_get_by_id(int user_id, User *out);
int user_repository_get_by_login(const char *login, User *out);
int user_repository_get_all(User **out);
int user_repository_update(int user_id, const char *login, const char *full_name);
int user_repository_update_password(int user_id, const char *password_hash);
int user_repository_delete(int user_id);

#endif
