#ifndef MODEL_STUDENT_H
#define MODEL_STUDENT_H

#include "sizes.h"

typedef struct {
    int  student_id;
    int  user_id;
    int  group_id;
    char record_book[RECORD_BOOK_LEN];

    char full_name[FULL_NAME_LEN];
    char group_name[GROUP_NAME_LEN];
} Student;

#endif
