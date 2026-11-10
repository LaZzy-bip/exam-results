#ifndef STUDENT_REPOSITORY_H
#define STUDENT_REPOSITORY_H

#include "../models/student.h"

int student_repository_create(int user_id, int group_id, const char *record_book); /* id или -1 */
int student_repository_get_by_id(int student_id, Student *out);                    /* 1 / 0 / -1 */
int student_repository_get_by_user_id(int user_id, Student *out);                  /* 1 / 0 / -1 */
int student_repository_get_by_record_book(const char *record_book, Student *out);  /* 1 / 0 / -1 */
int student_repository_get_all(Student **out);                                     /* кол-во или -1 */
int student_repository_get_by_group(int group_id, Student **out);                  /* кол-во или -1 */
int student_repository_update(int student_id, int group_id, const char *record_book); /* 0 / -1 */
int student_repository_delete(int student_id);                                     /* 0 / -1 */

#endif
