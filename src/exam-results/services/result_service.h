#ifndef RESULT_SERVICE_H
#define RESULT_SERVICE_H

#include "../models/exam_result.h"

int set_grade(int teacher_id, int exam_id, int student_id, int grade, char *err);
int update_grade(int teacher_id, int exam_id, int student_id, int grade, char *err);

int get_exam_statement(int teacher_id, int exam_id, ExamResult **out, char *err);

int search_results(int teacher_id, const char *record_book, int group_id, int subject_id,
                   ExamResult **out, char *err);

int get_student_results(int student_id, ExamResult **out);
int get_student_debts(int student_id, ExamResult **out);

int get_debts(int group_id, ExamResult **out);
int get_group_summary(int group_id, ExamResult **out, char *err);

int get_student_average(int student_id, double *average, int *count, char *err);
int get_group_average(int group_id, double *average, int *count, char *err);

int is_valid_grade(int grade);
int is_debt(const ExamResult *result);
int calc_average(const ExamResult *results, int n, double *average);

#endif
