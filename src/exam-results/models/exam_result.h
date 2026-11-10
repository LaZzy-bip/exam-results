#ifndef MODEL_EXAM_RESULT_H
#define MODEL_EXAM_RESULT_H

#include "sizes.h"

/*
 * Значения поля grade внутри программы:
 *   2..5          — оценка;
 *   GRADE_ABSENT  — «не явился» (в БД хранится как NULL);
 *   GRADE_NONE    — оценка ещё не выставлена (строки в exam_results нет,
 *                   встречается только в ведомости по экзамену).
 */
#define GRADE_ABSENT 0
#define GRADE_NONE   (-1)

/* Таблица exam_results (таблица-связка студентов и экзаменов) */
typedef struct {
    int result_id;
    int exam_id;
    int student_id;
    int grade;

    /* служебные поля, собранные из нескольких таблиц через JOIN */
    char student_name[FULL_NAME_LEN];
    char record_book[RECORD_BOOK_LEN];
    char group_name[GROUP_NAME_LEN];
    char subject_name[SUBJECT_LEN];
    char exam_date[DATE_LEN];
    char teacher_name[FULL_NAME_LEN];
} ExamResult;

#endif /* MODEL_EXAM_RESULT_H */
