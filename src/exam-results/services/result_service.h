#ifndef RESULT_SERVICE_H
#define RESULT_SERVICE_H

#include "../models/exam_result.h"

/*
 * Оценки выставляет только преподаватель, который ведёт экзамен.
 * grade: 2..5 или GRADE_ABSENT («не явился»).
 */
int set_grade(int teacher_id, int exam_id, int student_id, int grade, char *err);
int update_grade(int teacher_id, int exam_id, int student_id, int grade, char *err);

/* Ведомость по экзамену. teacher_id = 0 — без проверки владельца (для администратора). */
int get_exam_statement(int teacher_id, int exam_id, ExamResult **out, char *err); /* кол-во или -1 */

/*
 * Поиск результатов по экзаменам преподавателя.
 * record_book = "" / NULL, group_id = 0, subject_id = 0 — фильтр не применяется.
 */
int search_results(int teacher_id, const char *record_book, int group_id, int subject_id,
                   ExamResult **out, char *err);                                  /* кол-во или -1 */

/* Студент */
int get_student_results(int student_id, ExamResult **out);                        /* кол-во или -1 */
int get_student_debts(int student_id, ExamResult **out);                          /* кол-во или -1 */

/* Администратор: задолженности (group_id = 0 — по всем группам) и сводная ведомость */
int get_debts(int group_id, ExamResult **out);                                    /* кол-во или -1 */
int get_group_summary(int group_id, ExamResult **out, char *err);                 /* кол-во или -1 */

/*
 * Средний балл. В расчёт идут оценки 2–5, «не явился» не учитывается.
 * В *count записывается число оценок; при *count == 0 средний балл не определён.
 */
int get_student_average(int student_id, double *average, int *count, char *err);
int get_group_average(int group_id, double *average, int *count, char *err);

/* Бизнес-правила */
int is_valid_grade(int grade);                 /* 2..5 или «не явился» */
int is_debt(const ExamResult *result);         /* «2» или «не явился» */
int calc_average(const ExamResult *results, int n, double *average); /* возвращает число оценок */

#endif
