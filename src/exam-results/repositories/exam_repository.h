#ifndef EXAM_REPOSITORY_H
#define EXAM_REPOSITORY_H

#include "../models/exam.h"

int exam_repository_create(int subject_id, int teacher_id, int group_id,
                           const char *exam_date);              /* id или -1 */
int exam_repository_get_by_id(int exam_id, Exam *out);          /* 1 / 0 / -1 */
int exam_repository_get_all(Exam **out);                        /* кол-во или -1 */
int exam_repository_get_by_teacher(int teacher_id, Exam **out); /* кол-во или -1 */
int exam_repository_get_by_group(int group_id, Exam **out);     /* кол-во или -1 */
int exam_repository_update(int exam_id, int teacher_id, const char *exam_date); /* 0 / -1 */
int exam_repository_delete(int exam_id);                        /* 0 / -1 */

#endif
