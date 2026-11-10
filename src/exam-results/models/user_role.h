#ifndef MODEL_USER_ROLE_H
#define MODEL_USER_ROLE_H

#include "sizes.h"

#define ROLE_ADMIN   1
#define ROLE_TEACHER 2
#define ROLE_STUDENT 3

typedef struct {
    int  user_role_id;
    char name[ROLE_NAME_LEN];
} UserRole;

#endif
