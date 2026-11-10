#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "../models/user.h"
#include "../models/teacher.h"
#include "../models/student.h"

int register_teacher(const char *login, const char *password, const char *full_name,
                     const char *department, char *err);
int register_student(const char *login, const char *password, const char *full_name,
                     int group_id, const char *record_book, char *err);

int get_all_users(User **out);
int get_user(int user_id, User *out, char *err);
int get_all_teachers(Teacher **out);
int get_teacher_by_user(int user_id, Teacher *out, char *err);
int get_student_by_user(int user_id, Student *out, char *err);
int get_all_students(Student **out);
int get_students_by_group(int group_id, Student **out);

int update_user(int user_id, const char *new_login, const char *new_full_name, char *err);
int change_password(int user_id, const char *new_password, char *err);
int update_teacher_department(int user_id, const char *department, char *err);
int update_student_profile(int user_id, int new_group_id, const char *new_record_book, char *err);

int check_user_deletable(int user_id, int current_user_id, char *err);
int delete_user(int user_id, int current_user_id, char *err);

#endif
