#ifndef MODEL_EXAM_H
#define MODEL_EXAM_H

#include "sizes.h"

typedef struct {
    int  exam_id;
    int  subject_id;
    int  teacher_id;
    int  group_id;
    char exam_date[DATE_LEN];

    char subject_name[SUBJECT_LEN];
    char teacher_name[FULL_NAME_LEN];
    char group_name[GROUP_NAME_LEN];
} Exam;

#endif
