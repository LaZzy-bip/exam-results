#ifndef VIEWS_H
#define VIEWS_H

#include "../models/user.h"
#include "../models/student.h"
#include "../models/exam.h"
#include "../models/exam_result.h"

/* Вывод списков в виде таблиц. Функции show_* сами загружают данные через сервисы. */

int show_groups(void);     /* возвращает количество строк или -1 */
int show_subjects(void);
int show_teachers(void);
int show_all_students(void);

void print_users(const User *users, int n);
void print_students(const Student *students, int n);
void print_exams(const Exam *exams, int n);

/* Набор столбцов таблицы результатов (флаги можно объединять через |) */
#define COL_STUDENT_ID 1   /* ID студента — нужен преподавателю для выставления оценки */
#define COL_STUDENT    2   /* ФИО и номер зачётной книжки */
#define COL_GROUP      4
#define COL_EXAM       8   /* дисциплина и дата */
#define COL_TEACHER    16

void print_results(const ExamResult *results, int n, int columns);

#endif
