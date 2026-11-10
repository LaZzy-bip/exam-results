#include "teacher_menu.h"
#include "console.h"
#include "views.h"
#include "../models/teacher.h"
#include "../services/service_error.h"
#include "../services/user_service.h"
#include "../services/exam_service.h"
#include "../services/result_service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Список экзаменов преподавателя; возвращает их количество */
static int show_my_exams(const Teacher *teacher)
{
    Exam *exams = NULL;
    int n = get_exams_by_teacher(teacher->teacher_id, &exams);
    if (n < 0) {
        print_error("Не удалось загрузить экзамены");
        return -1;
    }
    print_exams(exams, n);
    free(exams);
    return n;
}

/* Выбор своего экзамена и вывод ведомости по нему; возвращает id экзамена или 0 */
static int choose_exam_and_show(const Teacher *teacher)
{
    int exam_id;
    char err[ERR_LEN];
    ExamResult *rows = NULL;

    if (show_my_exams(teacher) <= 0) return 0;
    if (!read_int("\nID экзамена: ", &exam_id)) return 0;

    int n = get_exam_statement(teacher->teacher_id, exam_id, &rows, err);
    if (n < 0) {
        print_error(err);
        return 0;
    }
    print_results(rows, n, COL_STUDENT_ID | COL_STUDENT);
    free(rows);
    return exam_id;
}

/* Ввод оценки: 2–5 или 0 — «не явился» */
static int read_grade(int *grade)
{
    int value;
    if (!read_int("Оценка (2–5, 0 — не явился): ", &value)) return 0;
    *grade = (value == 0) ? GRADE_ABSENT : value;
    return 1;
}

static void set_grades_form(const Teacher *teacher)
{
    char err[ERR_LEN];
    int exam_id = choose_exam_and_show(teacher);
    if (exam_id == 0) return;

    /* можно выставить оценки нескольким студентам подряд */
    while (1) {
        int student_id, grade;
        if (!read_int("\nID студента (Enter — закончить): ", &student_id)) return;
        if (!read_grade(&grade)) continue;

        if (set_grade(teacher->teacher_id, exam_id, student_id, grade, err) != SERVICE_OK) {
            print_error(err);
        } else {
            print_success("Оценка выставлена");
        }
    }
}

static void update_grade_form(const Teacher *teacher)
{
    char err[ERR_LEN];
    int student_id, grade;
    int exam_id = choose_exam_and_show(teacher);
    if (exam_id == 0) return;

    if (!read_int("\nID студента: ", &student_id)) return;
    if (!read_grade(&grade)) return;

    if (update_grade(teacher->teacher_id, exam_id, student_id, grade, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    print_success("Оценка изменена");
}

/* Группы и дисциплины, по которым у преподавателя есть экзамены (без повторов) */
static int choose_from_my_exams(const Teacher *teacher, int by_group, int *out_id)
{
    Exam *exams = NULL;
    int n = get_exams_by_teacher(teacher->teacher_id, &exams);
    if (n <= 0) {
        if (n == 0) printf("У вас нет экзаменов.\n");
        free(exams);
        return 0;
    }

    printf("\n");
    print_cell("ID", 4);
    print_cell(by_group ? "Группа" : "Дисциплина", 30);
    printf("\n");
    print_rule(36);
    for (int i = 0; i < n; i++) {
        int id = by_group ? exams[i].group_id : exams[i].subject_id;
        int seen = 0;
        for (int j = 0; j < i; j++) {
            if ((by_group ? exams[j].group_id : exams[j].subject_id) == id) {
                seen = 1;
                break;
            }
        }
        if (!seen) {
            print_cell_int(id, 4);
            print_cell(by_group ? exams[i].group_name : exams[i].subject_name, 30);
            printf("\n");
        }
    }
    free(exams);

    return read_int(by_group ? "ID группы: " : "ID дисциплины: ", out_id);
}

static void search_form(const Teacher *teacher)
{
    char choice[16];
    char record_book[RECORD_BOOK_LEN] = "";
    char err[ERR_LEN];
    int group_id = 0, subject_id = 0;

    print_title("Поиск результатов");
    printf("1. По студенту (номер зачётной книжки)\n");
    printf("2. По группе\n");
    printf("3. По дисциплине\n");
    printf("4. Все результаты по моим экзаменам\n");
    if (!read_line("Выберите: ", choice, sizeof choice)) return;

    if (strcmp(choice, "1") == 0) {
        if (!read_line("Номер зачётной книжки: ", record_book, sizeof record_book)) return;
        if (record_book[0] == '\0') return;
    } else if (strcmp(choice, "2") == 0) {
        if (!choose_from_my_exams(teacher, 1, &group_id)) return;
    } else if (strcmp(choice, "3") == 0) {
        if (!choose_from_my_exams(teacher, 0, &subject_id)) return;
    } else if (strcmp(choice, "4") != 0) {
        print_error("Нет такого пункта меню");
        return;
    }

    ExamResult *results = NULL;
    int n = search_results(teacher->teacher_id, record_book, group_id, subject_id, &results, err);
    if (n < 0) {
        print_error(err);
        return;
    }
    printf("\nНайдено записей: %d\n", n);
    print_results(results, n, COL_STUDENT | COL_GROUP | COL_EXAM);
    free(results);
}

static void statement_view(const Teacher *teacher)
{
    choose_exam_and_show(teacher);
}

void teacher_menu(const User *user)
{
    char err[ERR_LEN];
    char choice[16];
    char title[320];
    Teacher teacher;

    if (get_teacher_by_user(user->user_id, &teacher, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    snprintf(title, sizeof title, "Преподаватель: %s", teacher.full_name);

    while (1) {
        print_title(title);
        printf("1. Мои экзамены\n");
        printf("2. Выставить оценки\n");
        printf("3. Изменить оценку\n");
        printf("4. Ведомость по экзамену\n");
        printf("5. Поиск результатов\n");
        printf("0. Выйти из учётной записи\n");
        if (!read_line("Выберите действие: ", choice, sizeof choice)) return;

        if (strcmp(choice, "1") == 0)      show_my_exams(&teacher);
        else if (strcmp(choice, "2") == 0) set_grades_form(&teacher);
        else if (strcmp(choice, "3") == 0) update_grade_form(&teacher);
        else if (strcmp(choice, "4") == 0) statement_view(&teacher);
        else if (strcmp(choice, "5") == 0) search_form(&teacher);
        else if (strcmp(choice, "0") == 0) return;
        else print_error("Нет такого пункта меню");
    }
}
