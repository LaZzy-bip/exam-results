#ifndef CATALOG_SERVICE_H
#define CATALOG_SERVICE_H

#include "../models/subject.h"
#include "../models/student_group.h"

int create_subject(const char *name, char *err);
int get_all_subjects(Subject **out);
int get_subject(int subject_id, Subject *out, char *err);

int create_group(const char *name, char *err);
int get_all_groups(StudentGroup **out);
int get_group(int group_id, StudentGroup *out, char *err);

#endif
