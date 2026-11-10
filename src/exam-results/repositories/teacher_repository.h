#ifndef TEACHER_REPOSITORY_H
#define TEACHER_REPOSITORY_H

#include "../models/teacher.h"

int teacher_repository_create(int user_id, const char *department);
int teacher_repository_get_by_id(int teacher_id, Teacher *out);
int teacher_repository_get_by_user_id(int user_id, Teacher *out);
int teacher_repository_get_all(Teacher **out);
int teacher_repository_update(int teacher_id, const char *department);
int teacher_repository_delete(int teacher_id);

#endif
