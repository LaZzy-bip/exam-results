#ifndef SUBJECT_REPOSITORY_H
#define SUBJECT_REPOSITORY_H

#include "../models/subject.h"

int subject_repository_create(const char *name);                    /* id или -1 */
int subject_repository_get_by_id(int subject_id, Subject *out);     /* 1 / 0 / -1 */
int subject_repository_get_by_name(const char *name, Subject *out); /* 1 / 0 / -1 */
int subject_repository_get_all(Subject **out);                      /* кол-во или -1 */
int subject_repository_update(int subject_id, const char *name);    /* 0 / -1 */
int subject_repository_delete(int subject_id);                      /* 0 / -1 */

#endif
