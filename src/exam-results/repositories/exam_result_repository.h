#ifndef EXAM_RESULT_REPOSITORY_H
#define EXAM_RESULT_REPOSITORY_H

#include "../models/exam_result.h"

/* grade: 2..5 или GRADE_ABSENT («не явился», в БД записывается NULL) */
int exam_result_repository_create(int exam_id, int student_id, int grade);   /* id или -1 */
int exam_result_repository_get_by_id(int result_id, ExamResult *out);         /* 1 / 0 / -1 */
int exam_result_repository_get_by_exam_and_student(int exam_id, int student_id,
                                                   ExamResult *out);          /* 1 / 0 / -1 */
int exam_result_repository_get_all(ExamResult **out);                         /* кол-во или -1 */
int exam_result_repository_get_by_student(int student_id, ExamResult **out);  /* кол-во или -1 */
int exam_result_repository_get_by_group(int group_id, ExamResult **out);      /* кол-во или -1 */

/*
 * Результаты по экзаменам одного преподавателя с необязательными фильтрами.
 * Значение 0 в фильтре означает «не фильтровать».
 */
int exam_result_repository_search(int teacher_id, int student_id, int group_id,
                                  int subject_id, ExamResult **out);         /* кол-во или -1 */

/*
 * Ведомость экзамена: все студенты группы, в том числе те, кому оценка
 * ещё не выставлена (у них grade = GRADE_NONE, result_id = 0).
 */
int exam_result_repository_get_statement(int exam_id, ExamResult **out);      /* кол-во или -1 */

int exam_result_repository_count_by_exam(int exam_id);                        /* кол-во или -1 */
int exam_result_repository_update_grade(int result_id, int grade);            /* 0 / -1 */
int exam_result_repository_delete(int result_id);                             /* 0 / -1 */

#endif
