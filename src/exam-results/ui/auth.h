#ifndef AUTH_UI_H
#define AUTH_UI_H

#include "../models/user.h"

/* Вход в систему: 1 — успешно (out заполнен), 0 — нет. */
int login(User *out);

/* Регистрация пользователей (вызывается из меню администратора). */
void register_teacher_form(void);
void register_student_form(void);

#endif
