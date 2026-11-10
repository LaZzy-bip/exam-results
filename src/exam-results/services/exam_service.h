#ifndef EXAM_SERVICE_H
#define EXAM_SERVICE_H

#include "../models/exam.h"

/* Создание экзамена (открытие ведомости) администратором. Дата — ГГГГ-ММ-ДД. */
int create_exam(int subject_id, int group_id, int teacher_id, const char *exam_date, char *err);

int get_all_exams(Exam **out);                               /* кол-во или -1 */
int get_exams_by_teacher(int teacher_id, Exam **out);        /* кол-во или -1 */
int get_exams_by_group(int group_id, Exam **out);            /* кол-во или -1 */
int get_exam(int exam_id, Exam *out, char *err);

/* Экзамен, который принадлежит этому преподавателю (иначе ошибка доступа). */
int get_teacher_exam(int teacher_id, int exam_id, Exam *out, char *err);

/* Сколько оценок уже выставлено (чтобы предупредить перед удалением). */
int count_exam_results(int exam_id);

/* Удаление экзамена; оценки по нему удаляются каскадно. */
int delete_exam(int exam_id, char *err);

/* 1, если строка — корректная дата в формате ГГГГ-ММ-ДД. */
int is_valid_date(const char *date);

#endif
