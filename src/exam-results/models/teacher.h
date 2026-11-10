#ifndef MODEL_TEACHER_H
#define MODEL_TEACHER_H

#include "sizes.h"

/* Таблица teachers */
typedef struct {
    int  teacher_id;
    int  user_id;
    char department[DEPARTMENT_LEN];

    /* служебное поле: ФИО из users */
    char full_name[FULL_NAME_LEN];
} Teacher;

#endif /* MODEL_TEACHER_H */
