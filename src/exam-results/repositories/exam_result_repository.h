#ifndef EXAM_RESULT_REPOSITORY_H
#define EXAM_RESULT_REPOSITORY_H

#include "../models/exam_result.h"

int exam_result_repository_create(int exam_id, int student_id, int grade);
int exam_result_repository_get_by_id(int result_id, ExamResult *out);
int exam_result_repository_get_by_exam_and_student(int exam_id, int student_id,
                                                   ExamResult *out);
int exam_result_repository_get_all(ExamResult **out);
int exam_result_repository_get_by_student(int student_id, ExamResult **out);
int exam_result_repository_get_by_group(int group_id, ExamResult **out);

int exam_result_repository_search(int teacher_id, int student_id, int group_id,
                                  int subject_id, ExamResult **out);

int exam_result_repository_get_statement(int exam_id, ExamResult **out);

int exam_result_repository_count_by_exam(int exam_id);
int exam_result_repository_update_grade(int result_id, int grade);
int exam_result_repository_delete(int result_id);

#endif
