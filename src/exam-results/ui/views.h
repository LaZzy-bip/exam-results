#ifndef VIEWS_H
#define VIEWS_H

#include "../models/user.h"
#include "../models/student.h"
#include "../models/exam.h"
#include "../models/exam_result.h"

int show_groups(void);
int show_subjects(void);
int show_teachers(void);
int show_all_students(void);

void print_users(const User *users, int n);
void print_students(const Student *students, int n);
void print_exams(const Exam *exams, int n);

#define COL_STUDENT_ID 1
#define COL_STUDENT    2
#define COL_GROUP      4
#define COL_EXAM       8
#define COL_TEACHER    16

void print_results(const ExamResult *results, int n, int columns);

#endif
