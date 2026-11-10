#ifndef EXAM_SERVICE_H
#define EXAM_SERVICE_H

#include "../models/exam.h"

int create_exam(int subject_id, int group_id, int teacher_id, const char *exam_date, char *err);

int get_all_exams(Exam **out);
int get_exams_by_teacher(int teacher_id, Exam **out);
int get_exams_by_group(int group_id, Exam **out);
int get_exam(int exam_id, Exam *out, char *err);

int get_teacher_exam(int teacher_id, int exam_id, Exam *out, char *err);

int count_exam_results(int exam_id);

int delete_exam(int exam_id, char *err);

int is_valid_date(const char *date);

#endif
