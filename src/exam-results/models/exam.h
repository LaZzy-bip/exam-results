#ifndef MODEL_EXAM_H
#define MODEL_EXAM_H

#include "sizes.h"

/* Таблица exams (экзамен = открытая ведомость) */
typedef struct {
    int  exam_id;
    int  subject_id;
    int  teacher_id;
    int  group_id;
    char exam_date[DATE_LEN];

    /* служебные поля: названия вместо идентификаторов для вывода */
    char subject_name[SUBJECT_LEN];
    char teacher_name[FULL_NAME_LEN];
    char group_name[GROUP_NAME_LEN];
} Exam;

#endif /* MODEL_EXAM_H */
