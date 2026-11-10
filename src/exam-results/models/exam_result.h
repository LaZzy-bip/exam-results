#ifndef MODEL_EXAM_RESULT_H
#define MODEL_EXAM_RESULT_H

#include "sizes.h"

#define GRADE_ABSENT 0
#define GRADE_NONE   (-1)

typedef struct {
    int result_id;
    int exam_id;
    int student_id;
    int grade;

    char student_name[FULL_NAME_LEN];
    char record_book[RECORD_BOOK_LEN];
    char group_name[GROUP_NAME_LEN];
    char subject_name[SUBJECT_LEN];
    char exam_date[DATE_LEN];
    char teacher_name[FULL_NAME_LEN];
} ExamResult;

#endif
