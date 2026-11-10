#ifndef SUBJECT_REPOSITORY_H
#define SUBJECT_REPOSITORY_H

#include "../models/subject.h"

int subject_repository_create(const char *name);
int subject_repository_get_by_id(int subject_id, Subject *out);
int subject_repository_get_by_name(const char *name, Subject *out);
int subject_repository_get_all(Subject **out);
int subject_repository_update(int subject_id, const char *name);
int subject_repository_delete(int subject_id);

#endif
