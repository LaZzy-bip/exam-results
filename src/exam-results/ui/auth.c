#include "auth.h"
#include "console.h"
#include "views.h"
#include "../services/service_error.h"
#include "../services/auth_service.h"
#include "../services/user_service.h"

#include <stdio.h>

int login(User *out)
{
    char login_value[LOGIN_LEN];
    char password_value[64];
    char err[ERR_LEN];

    print_title("Вход в систему");
    if (!read_line("Логин: ", login_value, sizeof login_value)) return 0;
    if (!read_line("Пароль: ", password_value, sizeof password_value)) return 0;

    if (authenticate(login_value, password_value, out, err) != SERVICE_OK) {
        print_error(err);
        return 0;
    }
    printf("Здравствуйте, %s!\n", out->full_name);
    return 1;
}

/* Общие поля учётной записи */
static int read_account(char *login_value, size_t login_size,
                        char *password, size_t password_size,
                        char *full_name, size_t name_size)
{
    return read_line("Логин: ", login_value, login_size)
        && read_line("Пароль (6–32 латинских букв и цифр): ", password, password_size)
        && read_line("ФИО: ", full_name, name_size);
}

void register_teacher_form(void)
{
    char login_value[LOGIN_LEN], password[64], full_name[FULL_NAME_LEN];
    char department[DEPARTMENT_LEN];
    char err[ERR_LEN];

    print_title("Регистрация преподавателя");
    if (!read_account(login_value, sizeof login_value, password, sizeof password,
                      full_name, sizeof full_name)) {
        return;
    }
    if (!read_line("Кафедра: ", department, sizeof department)) return;

    if (register_teacher(login_value, password, full_name, department, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    print_success("Преподаватель зарегистрирован");
}

void register_student_form(void)
{
    char login_value[LOGIN_LEN], password[64], full_name[FULL_NAME_LEN];
    char record_book[RECORD_BOOK_LEN];
    char err[ERR_LEN];
    int group_id;

    print_title("Регистрация студента");
    if (!read_account(login_value, sizeof login_value, password, sizeof password,
                      full_name, sizeof full_name)) {
        return;
    }
    if (show_groups() <= 0) {
        print_error("Сначала добавьте учебную группу");
        return;
    }
    if (!read_int("ID группы: ", &group_id)) {
        print_error("Группа не выбрана");
        return;
    }
    if (!read_line("Номер зачётной книжки: ", record_book, sizeof record_book)) return;

    if (register_student(login_value, password, full_name, group_id, record_book, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    print_success("Студент зарегистрирован");
}
