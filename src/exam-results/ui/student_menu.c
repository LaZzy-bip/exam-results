#include "student_menu.h"
#include "console.h"
#include "views.h"
#include "../models/student.h"
#include "../services/service_error.h"
#include "../services/user_service.h"
#include "../services/result_service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void my_results(const Student *student)
{
    ExamResult *results = NULL;
    int n = get_student_results(student->student_id, &results);
    if (n < 0) {
        print_error("Не удалось загрузить результаты");
        return;
    }
    print_results(results, n, COL_EXAM | COL_TEACHER);
    free(results);
}

static void my_debts(const Student *student)
{
    ExamResult *debts = NULL;
    int n = get_student_debts(student->student_id, &debts);
    if (n < 0) {
        print_error("Не удалось загрузить задолженности");
        return;
    }
    if (n == 0) {
        printf("Задолженностей нет.\n");
    } else {
        printf("Задолженностей: %d\n", n);
        print_results(debts, n, COL_EXAM | COL_TEACHER);
    }
    free(debts);
}

static void my_average(const Student *student)
{
    char err[ERR_LEN];
    double avg;
    int count;
    if (get_student_average(student->student_id, &avg, &count, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    if (count == 0) {
        printf("Оценок пока нет.\n");
    } else {
        printf("Ваш средний балл: %.2f (учтено оценок: %d)\n", avg, count);
    }
}

void student_menu(const User *user)
{
    char err[ERR_LEN];
    char choice[16];
    char title[320];
    Student student;

    if (get_student_by_user(user->user_id, &student, err) != SERVICE_OK) {
        print_error(err);
        return;
    }
    snprintf(title, sizeof title, "Студент: %s (%s)", student.full_name, student.group_name);

    while (1) {
        print_title(title);
        printf("1. Мои результаты экзаменов\n");
        printf("2. Мои задолженности\n");
        printf("3. Мой средний балл\n");
        printf("0. Выйти из учётной записи\n");
        if (!read_line("Выберите действие: ", choice, sizeof choice)) return;

        if (strcmp(choice, "1") == 0)      my_results(&student);
        else if (strcmp(choice, "2") == 0) my_debts(&student);
        else if (strcmp(choice, "3") == 0) my_average(&student);
        else if (strcmp(choice, "0") == 0) return;
        else print_error("Нет такого пункта меню");
    }
}
