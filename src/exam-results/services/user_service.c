#include "user_service.h"
#include "service_error.h"
#include "../db.h"
#include "../common/cipher.h"
#include "../common/str_utils.h"
#include "../models/user_role.h"
#include "../models/exam.h"
#include "../repositories/user_repository.h"
#include "../repositories/teacher_repository.h"
#include "../repositories/student_repository.h"
#include "../repositories/group_repository.h"
#include "../repositories/exam_repository.h"

#include <stdlib.h>
#include <string.h>

#define MIN_LOGIN_LEN    3
#define MAX_LOGIN_LEN    30
#define MIN_PASSWORD_LEN 6
#define MAX_PASSWORD_LEN 32

/* ---------- проверки данных ---------- */

static int validate_login(const char *login, int except_user_id, char *err)
{
    if (str_is_blank(login)) {
        return service_fail(err, "Логин не может быть пустым");
    }
    if (str_has_space(login)) {
        return service_fail(err, "Логин не должен содержать пробелы");
    }
    size_t len = utf8_length(login);
    if (len < MIN_LOGIN_LEN || len > MAX_LOGIN_LEN) {
        return service_fail(err, "Длина логина — от 3 до 30 символов");
    }

    User existing;
    int found = user_repository_get_by_login(login, &existing);
    if (found < 0) {
        return service_fail(err, "Ошибка базы данных");
    }
    if (found == 1 && existing.user_id != except_user_id) {
        return service_fail(err, "Пользователь с таким логином уже существует");
    }
    return SERVICE_OK;
}

static int validate_password(const char *password, char *err)
{
    size_t len = (password == NULL) ? 0 : strlen(password);
    if (len < MIN_PASSWORD_LEN || len > MAX_PASSWORD_LEN) {
        return service_fail(err, "Длина пароля — от 6 до 32 символов");
    }
    /* шифр работает только с латинскими буквами и цифрами */
    if (!is_cipher_alphabet(password)) {
        return service_fail(err, "Пароль может содержать только латинские буквы и цифры");
    }
    return SERVICE_OK;
}

static int validate_full_name(const char *full_name, char *err)
{
    if (str_is_blank(full_name)) {
        return service_fail(err, "ФИО не может быть пустым");
    }
    if (strlen(full_name) >= FULL_NAME_LEN) {
        return service_fail(err, "Слишком длинное ФИО");
    }
    return SERVICE_OK;
}

static int validate_group(int group_id, char *err)
{
    StudentGroup group;
    int found = group_repository_get_by_id(group_id, &group);
    if (found < 0) {
        return service_fail(err, "Ошибка базы данных");
    }
    if (found == 0) {
        return service_fail(err, "Группа не найдена");
    }
    return SERVICE_OK;
}

static int validate_record_book(const char *record_book, int except_student_id, char *err)
{
    if (str_is_blank(record_book)) {
        return service_fail(err, "Номер зачётной книжки не может быть пустым");
    }
    if (strlen(record_book) >= RECORD_BOOK_LEN) {
        return service_fail(err, "Слишком длинный номер зачётной книжки");
    }

    Student existing;
    int found = student_repository_get_by_record_book(record_book, &existing);
    if (found < 0) {
        return service_fail(err, "Ошибка базы данных");
    }
    if (found == 1 && existing.student_id != except_student_id) {
        return service_fail(err, "Зачётная книжка с таким номером уже есть у другого студента");
    }
    return SERVICE_OK;
}

static int validate_new_user(const char *login, const char *password,
                             const char *full_name, char *err)
{
    if (validate_login(login, 0, err) != SERVICE_OK) return SERVICE_ERROR;
    if (validate_password(password, err) != SERVICE_OK) return SERVICE_ERROR;
    if (validate_full_name(full_name, err) != SERVICE_OK) return SERVICE_ERROR;
    return SERVICE_OK;
}

/* ---------- регистрация ---------- */

int register_teacher(const char *login, const char *password, const char *full_name,
                     const char *department, char *err)
{
    if (validate_new_user(login, password, full_name, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }
    if (str_is_blank(department) || strlen(department) >= DEPARTMENT_LEN) {
        return service_fail(err, "Укажите кафедру преподавателя");
    }

    char encrypted[PASSWORD_LEN];
    encrypt_password(password, encrypted, sizeof encrypted);

    /* users и teachers заполняются вместе: если второй INSERT не пройдёт, откатываем первый */
    db_begin();
    int user_id = user_repository_create(login, encrypted, full_name, ROLE_TEACHER);
    if (user_id < 0 || teacher_repository_create(user_id, department) < 0) {
        db_rollback();
        return service_fail(err, "Не удалось сохранить преподавателя");
    }
    db_commit();
    return SERVICE_OK;
}

int register_student(const char *login, const char *password, const char *full_name,
                     int group_id, const char *record_book, char *err)
{
    if (validate_new_user(login, password, full_name, err) != SERVICE_OK) return SERVICE_ERROR;
    if (validate_group(group_id, err) != SERVICE_OK) return SERVICE_ERROR;
    if (validate_record_book(record_book, 0, err) != SERVICE_OK) return SERVICE_ERROR;

    char encrypted[PASSWORD_LEN];
    encrypt_password(password, encrypted, sizeof encrypted);

    db_begin();
    int user_id = user_repository_create(login, encrypted, full_name, ROLE_STUDENT);
    if (user_id < 0 || student_repository_create(user_id, group_id, record_book) < 0) {
        db_rollback();
        return service_fail(err, "Не удалось сохранить студента");
    }
    db_commit();
    return SERVICE_OK;
}

/* ---------- чтение ---------- */

int get_all_users(User **out)
{
    return user_repository_get_all(out);
}

int get_user(int user_id, User *out, char *err)
{
    int found = user_repository_get_by_id(user_id, out);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) return service_fail(err, "Пользователь не найден");
    return SERVICE_OK;
}

int get_all_teachers(Teacher **out)
{
    return teacher_repository_get_all(out);
}

int get_teacher_by_user(int user_id, Teacher *out, char *err)
{
    int found = teacher_repository_get_by_user_id(user_id, out);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) return service_fail(err, "Профиль преподавателя не найден");
    return SERVICE_OK;
}

int get_student_by_user(int user_id, Student *out, char *err)
{
    int found = student_repository_get_by_user_id(user_id, out);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) return service_fail(err, "Профиль студента не найден");
    return SERVICE_OK;
}

int get_all_students(Student **out)
{
    return student_repository_get_all(out);
}

int get_students_by_group(int group_id, Student **out)
{
    return student_repository_get_by_group(group_id, out);
}

/* ---------- изменение ---------- */

/* Редактировать через меню можно только преподавателей и студентов */
static int load_editable_user(int user_id, User *user, char *err)
{
    if (get_user(user_id, user, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }
    if (user->user_role == ROLE_ADMIN) {
        return service_fail(err, "Учётная запись администратора не редактируется");
    }
    return SERVICE_OK;
}

int update_user(int user_id, const char *new_login, const char *new_full_name, char *err)
{
    User user;
    if (load_editable_user(user_id, &user, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }

    const char *login = str_is_blank(new_login) ? user.login : new_login;
    const char *full_name = str_is_blank(new_full_name) ? user.full_name : new_full_name;

    if (validate_login(login, user_id, err) != SERVICE_OK) return SERVICE_ERROR;
    if (validate_full_name(full_name, err) != SERVICE_OK) return SERVICE_ERROR;

    if (user_repository_update(user_id, login, full_name) != 0) {
        return service_fail(err, "Не удалось сохранить изменения");
    }
    return SERVICE_OK;
}

int change_password(int user_id, const char *new_password, char *err)
{
    User user;
    if (load_editable_user(user_id, &user, err) != SERVICE_OK) return SERVICE_ERROR;
    if (validate_password(new_password, err) != SERVICE_OK) return SERVICE_ERROR;

    char encrypted[PASSWORD_LEN];
    encrypt_password(new_password, encrypted, sizeof encrypted);
    if (user_repository_update_password(user_id, encrypted) != 0) {
        return service_fail(err, "Не удалось изменить пароль");
    }
    return SERVICE_OK;
}

int update_teacher_department(int user_id, const char *department, char *err)
{
    Teacher teacher;
    if (get_teacher_by_user(user_id, &teacher, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }
    if (str_is_blank(department)) {
        return SERVICE_OK; /* оставить прежнюю кафедру */
    }
    if (strlen(department) >= DEPARTMENT_LEN) {
        return service_fail(err, "Слишком длинное название кафедры");
    }
    if (teacher_repository_update(teacher.teacher_id, department) != 0) {
        return service_fail(err, "Не удалось сохранить изменения");
    }
    return SERVICE_OK;
}

int update_student_profile(int user_id, int new_group_id, const char *new_record_book, char *err)
{
    Student student;
    if (get_student_by_user(user_id, &student, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }

    int group_id = (new_group_id > 0) ? new_group_id : student.group_id;
    const char *record_book = str_is_blank(new_record_book) ? student.record_book : new_record_book;

    if (validate_group(group_id, err) != SERVICE_OK) return SERVICE_ERROR;
    if (validate_record_book(record_book, student.student_id, err) != SERVICE_OK) return SERVICE_ERROR;

    if (student_repository_update(student.student_id, group_id, record_book) != 0) {
        return service_fail(err, "Не удалось сохранить изменения");
    }
    return SERVICE_OK;
}

/* ---------- удаление ---------- */

int check_user_deletable(int user_id, int current_user_id, char *err)
{
    User user;
    if (get_user(user_id, &user, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }
    if (user_id == current_user_id) {
        return service_fail(err, "Нельзя удалить собственную учётную запись");
    }
    if (user.user_role == ROLE_ADMIN) {
        return service_fail(err, "Учётную запись администратора удалить нельзя");
    }

    if (user.user_role == ROLE_TEACHER) {
        Teacher teacher;
        if (get_teacher_by_user(user_id, &teacher, err) != SERVICE_OK) {
            return SERVICE_ERROR;
        }
        Exam *exams = NULL;
        int n = exam_repository_get_by_teacher(teacher.teacher_id, &exams);
        free(exams);
        if (n < 0) {
            return service_fail(err, "Ошибка базы данных");
        }
        if (n > 0) {
            return service_fail(err, "У преподавателя есть экзамены, сначала удалите их");
        }
    }
    return SERVICE_OK;
}

int delete_user(int user_id, int current_user_id, char *err)
{
    if (check_user_deletable(user_id, current_user_id, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }

    /* профиль в teachers / students и оценки студента удалятся каскадно (ON DELETE CASCADE) */
    if (user_repository_delete(user_id) != 0) {
        return service_fail(err, "Не удалось удалить пользователя");
    }
    return SERVICE_OK;
}
