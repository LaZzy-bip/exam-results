#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "../models/user.h"
#include "../models/teacher.h"
#include "../models/student.h"

/*
 * Регистрация пользователей выполняется только администратором.
 * Учётная запись и профиль (teachers / students) создаются в одной транзакции.
 * Возвращают SERVICE_OK или SERVICE_ERROR (текст ошибки — в err).
 */
int register_teacher(const char *login, const char *password, const char *full_name,
                     const char *department, char *err);
int register_student(const char *login, const char *password, const char *full_name,
                     int group_id, const char *record_book, char *err);

/* Чтение */
int get_all_users(User **out);                                  /* кол-во или -1 */
int get_user(int user_id, User *out, char *err);
int get_all_teachers(Teacher **out);                            /* кол-во или -1 */
int get_teacher_by_user(int user_id, Teacher *out, char *err);
int get_student_by_user(int user_id, Student *out, char *err);
int get_all_students(Student **out);                            /* кол-во или -1 */
int get_students_by_group(int group_id, Student **out);         /* кол-во или -1 */

/* Изменение (пустая строка = оставить прежнее значение) */
int update_user(int user_id, const char *new_login, const char *new_full_name, char *err);
int change_password(int user_id, const char *new_password, char *err);
int update_teacher_department(int user_id, const char *department, char *err);
int update_student_profile(int user_id, int new_group_id, const char *new_record_book, char *err);

/*
 * Удаление. Администратора удалить нельзя, преподавателя с экзаменами — тоже.
 * check_user_deletable() вызывается до вопроса «Удалить?», чтобы сразу сообщить о запрете.
 */
int check_user_deletable(int user_id, int current_user_id, char *err);
int delete_user(int user_id, int current_user_id, char *err);

#endif
