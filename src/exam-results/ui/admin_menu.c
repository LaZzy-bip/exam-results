#include "admin_menu.h"
#include "auth.h"
#include "console.h"
#include "views.h"
#include "../models/user_role.h"
#include "../services/service_error.h"
#include "../services/user_service.h"
#include "../services/catalog_service.h"
#include "../services/exam_service.h"
#include "../services/result_service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void list_users(void)
{
    User *users = NULL;
    int n = get_all_users(&users);
    if (n < 0) {
        print_error("Не удалось загрузить пользователей");
        return;
    }
    print_users(users, n);
    free(users);
}

static void edit_user_form(void)
{
    char err[ERR_LEN];
    char buf[FULL_NAME_LEN];
    char login_value[LOGIN_LEN];
    int user_id;
    User user;

    list_users();
    if (!read_int("\nID пользователя: ", &user_id)) return;
    if (get_user(user_id, &user, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    if (user.user_role == ROLE_ADMIN) {
        print_error("Учётная запись администратора не редактируется");
        return;
    }

    printf("Пустой ввод (Enter) — оставить прежнее значение.\n");

    printf("Логин [%s]: ", user.login);
    if (!read_line(NULL, login_value, sizeof login_value)) return;
    printf("ФИО [%s]: ", user.full_name);
    if (!read_line(NULL, buf, sizeof buf)) return;
    if (update_user(user_id, login_value, buf, err) != SERVICE_OK) {
        print_error(err);
        return;
    }

    if (user.user_role == ROLE_TEACHER) {
        Teacher teacher;
        if (get_teacher_by_user(user_id, &teacher, err) == SERVICE_OK) {
            char department[DEPARTMENT_LEN];
            printf("Кафедра [%s]: ", teacher.department);
            if (!read_line(NULL, department, sizeof department)) return;
            if (update_teacher_department(user_id, department, err) != SERVICE_OK) {
                print_error(err);
                return;
            }
        }
    } else if (user.user_role == ROLE_STUDENT) {
        Student student;
        if (get_student_by_user(user_id, &student, err) == SERVICE_OK) {
            int group_id = 0;
            char record_book[RECORD_BOOK_LEN];
            show_groups();
            printf("ID группы [%d — %s]: ", student.group_id, student.group_name);
            read_int(NULL, &group_id);
            printf("Номер зачётной книжки [%s]: ", student.record_book);
            if (!read_line(NULL, record_book, sizeof record_book)) return;
            if (update_student_profile(user_id, group_id, record_book, err) != SERVICE_OK) {
                print_error(err);
                return;
            }
        }
    }

    if (confirm("Сменить пароль?")) {
        char password[64];
        if (!read_line("Новый пароль: ", password, sizeof password)) return;
        if (change_password(user_id, password, err) != SERVICE_OK) {
            print_error(err);
            return;
        }
    }
    print_success("Данные пользователя сохранены");
}

static void delete_user_form(const User *admin)
{
    char err[ERR_LEN];
    int user_id;
    User user;

    list_users();
    if (!read_int("\nID пользователя: ", &user_id)) return;
    if (get_user(user_id, &user, err) != SERVICE_OK
        || check_user_deletable(user_id, admin->user_id, err) != SERVICE_OK) {
        print_error(err);
        return;
    }

    printf("Будет удалён пользователь: %s (%s).\n", user.full_name, user.role_name);
    if (user.user_role == ROLE_STUDENT) {
        printf("Вместе с ним будут удалены все его оценки.\n");
    }
    if (!confirm("Удалить?")) {
        printf("Отменено.\n");
        return;
    }
    if (delete_user(user_id, admin->user_id, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    print_success("Пользователь удалён");
}

static void users_menu(const User *admin)
{
    char choice[16];
    while (1) {
        print_title("Пользователи");
        printf("1. Список пользователей\n");
        printf("2. Зарегистрировать преподавателя\n");
        printf("3. Зарегистрировать студента\n");
        printf("4. Редактировать пользователя\n");
        printf("5. Удалить пользователя\n");
        printf("0. Назад\n");
        if (!read_line("Выберите действие: ", choice, sizeof choice)) return;

        if (strcmp(choice, "1") == 0)      list_users();
        else if (strcmp(choice, "2") == 0) register_teacher_form();
        else if (strcmp(choice, "3") == 0) register_student_form();
        else if (strcmp(choice, "4") == 0) edit_user_form();
        else if (strcmp(choice, "5") == 0) delete_user_form(admin);
        else if (strcmp(choice, "0") == 0) return;
        else print_error("Нет такого пункта меню");
    }
}

static void add_subject_form(void)
{
    char name[SUBJECT_LEN];
    char err[ERR_LEN];
    if (!read_line("Название дисциплины: ", name, sizeof name)) return;
    if (create_subject(name, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    print_success("Дисциплина добавлена");
}

static void add_group_form(void)
{
    char name[GROUP_NAME_LEN];
    char err[ERR_LEN];
    if (!read_line("Название группы: ", name, sizeof name)) return;
    if (create_group(name, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    print_success("Группа добавлена");
}

static void catalog_menu(void)
{
    char choice[16];
    while (1) {
        print_title("Дисциплины и группы");
        printf("1. Список дисциплин\n");
        printf("2. Добавить дисциплину\n");
        printf("3. Список групп\n");
        printf("4. Добавить группу\n");
        printf("0. Назад\n");
        if (!read_line("Выберите действие: ", choice, sizeof choice)) return;

        if (strcmp(choice, "1") == 0)      show_subjects();
        else if (strcmp(choice, "2") == 0) add_subject_form();
        else if (strcmp(choice, "3") == 0) show_groups();
        else if (strcmp(choice, "4") == 0) add_group_form();
        else if (strcmp(choice, "0") == 0) return;
        else print_error("Нет такого пункта меню");
    }
}

static void list_exams(void)
{
    Exam *exams = NULL;
    int n = get_all_exams(&exams);
    if (n < 0) {
        print_error("Не удалось загрузить экзамены");
        return;
    }
    print_exams(exams, n);
    free(exams);
}

static void create_exam_form(void)
{
    int subject_id, group_id, teacher_id;
    char date[32];
    char err[ERR_LEN];

    print_title("Создание экзамена (открытие ведомости)");
    if (show_subjects() <= 0 || !read_int("ID дисциплины: ", &subject_id)) return;
    if (show_groups() <= 0 || !read_int("ID группы: ", &group_id)) return;
    if (show_teachers() <= 0 || !read_int("ID преподавателя: ", &teacher_id)) return;
    if (!read_line("Дата экзамена (ГГГГ-ММ-ДД): ", date, sizeof date)) return;

    if (create_exam(subject_id, group_id, teacher_id, date, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    print_success("Экзамен создан, ведомость открыта");
}

static void delete_exam_form(void)
{
    int exam_id;
    char err[ERR_LEN];
    Exam exam;

    list_exams();
    if (!read_int("\nID экзамена: ", &exam_id)) return;
    if (get_exam(exam_id, &exam, err) != SERVICE_OK) {
        print_error(err);
        return;
    }

    int grades = count_exam_results(exam_id);
    printf("Экзамен: %s, группа %s, %s.\n", exam.subject_name, exam.group_name, exam.exam_date);
    if (grades > 0) {
        printf("По нему выставлено оценок: %d. Они тоже будут удалены.\n", grades);
    }
    if (!confirm("Удалить экзамен?")) {
        printf("Отменено.\n");
        return;
    }
    if (delete_exam(exam_id, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    print_success("Экзамен удалён");
}

static void exam_statement_view(void)
{
    int exam_id;
    char err[ERR_LEN];
    ExamResult *rows = NULL;

    list_exams();
    if (!read_int("\nID экзамена: ", &exam_id)) return;
    int n = get_exam_statement(0, exam_id, &rows, err);
    if (n < 0) {
        print_error(err);
        return;
    }
    print_results(rows, n, COL_STUDENT);
    free(rows);
}

static void exams_menu(void)
{
    char choice[16];
    while (1) {
        print_title("Экзамены");
        printf("1. Список экзаменов\n");
        printf("2. Создать экзамен\n");
        printf("3. Удалить экзамен\n");
        printf("4. Ведомость по экзамену\n");
        printf("0. Назад\n");
        if (!read_line("Выберите действие: ", choice, sizeof choice)) return;

        if (strcmp(choice, "1") == 0)      list_exams();
        else if (strcmp(choice, "2") == 0) create_exam_form();
        else if (strcmp(choice, "3") == 0) delete_exam_form();
        else if (strcmp(choice, "4") == 0) exam_statement_view();
        else if (strcmp(choice, "0") == 0) return;
        else print_error("Нет такого пункта меню");
    }
}

static int find_grade(const ExamResult *results, int n, int student_id, int exam_id)
{
    for (int i = 0; i < n; i++) {
        if (results[i].student_id == student_id && results[i].exam_id == exam_id) {
            return results[i].grade;
        }
    }
    return GRADE_NONE;
}

static void group_summary_view(void)
{
    int group_id;
    char err[ERR_LEN];
    StudentGroup group;

    if (show_groups() <= 0 || !read_int("ID группы: ", &group_id)) return;
    if (get_group(group_id, &group, err) != SERVICE_OK) {
        print_error(err);
        return;
    }

    Student *students = NULL;
    Exam *exams = NULL;
    ExamResult *results = NULL;
    int ns = get_students_by_group(group_id, &students);
    int ne = get_exams_by_group(group_id, &exams);
    int nr = get_group_summary(group_id, &results, err);

    if (ns < 0 || ne < 0 || nr < 0) {
        print_error("Не удалось сформировать ведомость");
    } else if (ns == 0 || ne == 0) {
        printf("В группе %s нет студентов или экзаменов.\n", group.name);
    } else {
        printf("\nСводная ведомость группы %s\n\n", group.name);
        print_cell("Студент", 28);
        for (int j = 0; j < ne; j++) {
            print_cell(exams[j].subject_name, 14);
        }
        print_cell("Ср. балл", 8);
        printf("\n");
        print_rule(29 + 15 * ne + 9);

        for (int i = 0; i < ns; i++) {
            print_cell(students[i].full_name, 28);
            for (int j = 0; j < ne; j++) {
                print_cell(grade_short(find_grade(results, nr, students[i].student_id,
                                                  exams[j].exam_id)), 14);
            }
            double avg;
            int count;
            if (get_student_average(students[i].student_id, &avg, &count, err) == SERVICE_OK
                && count > 0) {
                printf("%.2f", avg);
            } else {
                printf("—");
            }
            printf("\n");
        }

        double group_avg;
        int count;
        if (get_group_average(group_id, &group_avg, &count, err) == SERVICE_OK && count > 0) {
            printf("\nСредний балл группы: %.2f (оценок: %d)\n", group_avg, count);
        }
        printf("н/я — не явился, — — оценка не выставлена\n");
    }

    free(students);
    free(exams);
    free(results);
}

static void debts_view(void)
{
    int group_id = 0;
    ExamResult *debts = NULL;

    show_groups();
    printf("ID группы (Enter — все группы): ");
    read_int(NULL, &group_id);

    int n = get_debts(group_id, &debts);
    if (n < 0) {
        print_error("Не удалось загрузить задолженности");
        return;
    }
    printf("\nЗадолженности (оценка «2» или неявка): %d\n", n);
    print_results(debts, n, COL_STUDENT | COL_GROUP | COL_EXAM);
    free(debts);
}

static void average_view(void)
{
    char choice[16];
    char err[ERR_LEN];
    double avg;
    int count, id;

    printf("1. Средний балл студента\n");
    printf("2. Средний балл группы\n");
    if (!read_line("Выберите: ", choice, sizeof choice)) return;

    if (strcmp(choice, "1") == 0) {
        if (show_all_students() <= 0 || !read_int("ID студента: ", &id)) return;
        if (get_student_average(id, &avg, &count, err) != SERVICE_OK) {
            print_error(err);
            return;
        }
    } else if (strcmp(choice, "2") == 0) {
        if (show_groups() <= 0 || !read_int("ID группы: ", &id)) return;
        if (get_group_average(id, &avg, &count, err) != SERVICE_OK) {
            print_error(err);
            return;
        }
    } else {
        print_error("Нет такого пункта меню");
        return;
    }

    if (count == 0) {
        printf("Оценок пока нет, средний балл не рассчитан.\n");
    } else {
        printf("Средний балл: %.2f (учтено оценок: %d)\n", avg, count);
    }
}

void admin_menu(const User *user)
{
    char choice[16];
    char title[128];
    snprintf(title, sizeof title, "Администратор (%s)", user->login);

    while (1) {
        print_title(title);
        printf("1. Пользователи\n");
        printf("2. Дисциплины и группы\n");
        printf("3. Экзамены\n");
        printf("4. Сводная ведомость по группе\n");
        printf("5. Задолженности\n");
        printf("6. Средний балл\n");
        printf("0. Выйти из учётной записи\n");
        if (!read_line("Выберите действие: ", choice, sizeof choice)) return;

        if (strcmp(choice, "1") == 0)      users_menu(user);
        else if (strcmp(choice, "2") == 0) catalog_menu();
        else if (strcmp(choice, "3") == 0) exams_menu();
        else if (strcmp(choice, "4") == 0) group_summary_view();
        else if (strcmp(choice, "5") == 0) debts_view();
        else if (strcmp(choice, "6") == 0) average_view();
        else if (strcmp(choice, "0") == 0) return;
        else print_error("Нет такого пункта меню");
    }
}
