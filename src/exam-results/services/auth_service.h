#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "../models/user.h"

int authenticate(const char *login, const char *password, User *out, char *err);

#endif
