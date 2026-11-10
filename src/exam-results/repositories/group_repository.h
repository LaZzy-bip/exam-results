#ifndef GROUP_REPOSITORY_H
#define GROUP_REPOSITORY_H

#include "../models/student_group.h"

int group_repository_create(const char *name);
int group_repository_get_by_id(int group_id, StudentGroup *out);
int group_repository_get_by_name(const char *name, StudentGroup *out);
int group_repository_get_all(StudentGroup **out);
int group_repository_update(int group_id, const char *name);
int group_repository_delete(int group_id);

#endif
