#ifndef AUTH_UI_H
#define AUTH_UI_H

#include "../models/user.h"

int login(User *out);

void register_teacher_form(void);
void register_student_form(void);

#endif
