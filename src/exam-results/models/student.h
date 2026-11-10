#ifndef MODEL_STUDENT_H
#define MODEL_STUDENT_H

#include "sizes.h"

/* Таблица students */
typedef struct {
    int  student_id;
    int  user_id;
    int  group_id;
    char record_book[RECORD_BOOK_LEN];

    /* служебные поля: ФИО из users и название группы из student_groups */
    char full_name[FULL_NAME_LEN];
    char group_name[GROUP_NAME_LEN];
} Student;

#endif /* MODEL_STUDENT_H */
