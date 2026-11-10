#ifndef MODEL_STUDENT_GROUP_H
#define MODEL_STUDENT_GROUP_H

#include "sizes.h"

/* Таблица student_groups */
typedef struct {
    int  group_id;
    char name[GROUP_NAME_LEN];
} StudentGroup;

#endif /* MODEL_STUDENT_GROUP_H */
