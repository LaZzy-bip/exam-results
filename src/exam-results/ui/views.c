#include "views.h"
#include "console.h"
#include "../services/user_service.h"
#include "../services/catalog_service.h"

#include <stdio.h>
#include <stdlib.h>

int show_groups(void)
{
    StudentGroup *groups = NULL;
    int n = get_all_groups(&groups);
    if (n < 0) {
        print_error("Не удалось загрузить группы");
        return -1;
    }
    printf("\n");
    print_cell("ID", 4); print_cell("Группа", 20); printf("\n");
    print_rule(26);
    for (int i = 0; i < n; i++) {
        print_cell_int(groups[i].group_id, 4);
        print_cell(groups[i].name, 20);
        printf("\n");
    }
    free(groups);
    return n;
}

int show_subjects(void)
{
    Subject *subjects = NULL;
    int n = get_all_subjects(&subjects);
    if (n < 0) {
        print_error("Не удалось загрузить дисциплины");
        return -1;
    }
    printf("\n");
    print_cell("ID", 4); print_cell("Дисциплина", 40); printf("\n");
    print_rule(46);
    for (int i = 0; i < n; i++) {
        print_cell_int(subjects[i].subject_id, 4);
        print_cell(subjects[i].name, 40);
        printf("\n");
    }
    free(subjects);
    return n;
}

int show_teachers(void)
{
    Teacher *teachers = NULL;
    int n = get_all_teachers(&teachers);
    if (n < 0) {
        print_error("Не удалось загрузить преподавателей");
        return -1;
    }
    printf("\n");
    print_cell("ID", 4); print_cell("ФИО", 30); print_cell("Кафедра", 44); printf("\n");
    print_rule(80);
    for (int i = 0; i < n; i++) {
        print_cell_int(teachers[i].teacher_id, 4);
        print_cell(teachers[i].full_name, 30);
        print_cell(teachers[i].department, 44);
        printf("\n");
    }
    free(teachers);
    return n;
}

int show_all_students(void)
{
    Student *students = NULL;
    int n = get_all_students(&students);
    if (n < 0) {
        print_error("Не удалось загрузить студентов");
        return -1;
    }
    print_students(students, n);
    free(students);
    return n;
}

void print_users(const User *users, int n)
{
    printf("\n");
    print_cell("ID", 4); print_cell("Логин", 12); print_cell("ФИО", 30);
    print_cell("Роль", 14); print_cell("Группа / кафедра", 30); printf("\n");
    print_rule(94);

    for (int i = 0; i < n; i++) {
        char extra[DEPARTMENT_LEN] = "";
        Teacher teacher;
        Student student;

        if (get_teacher_by_user(users[i].user_id, &teacher, NULL) == 0) {
            snprintf(extra, sizeof extra, "%s", teacher.department);
        } else if (get_student_by_user(users[i].user_id, &student, NULL) == 0) {
            snprintf(extra, sizeof extra, "%s, з/к %s", student.group_name, student.record_book);
        }

        print_cell_int(users[i].user_id, 4);
        print_cell(users[i].login, 12);
        print_cell(users[i].full_name, 30);
        print_cell(users[i].role_name, 14);
        print_cell(extra, 30);
        printf("\n");
    }
}

void print_students(const Student *students, int n)
{
    printf("\n");
    print_cell("ID", 4); print_cell("ФИО", 30); print_cell("Группа", 10);
    print_cell("Зачётка", 12); printf("\n");
    print_rule(60);
    for (int i = 0; i < n; i++) {
        print_cell_int(students[i].student_id, 4);
        print_cell(students[i].full_name, 30);
        print_cell(students[i].group_name, 10);
        print_cell(students[i].record_book, 12);
        printf("\n");
    }
}

void print_exams(const Exam *exams, int n)
{
    if (n == 0) {
        printf("Экзаменов нет.\n");
        return;
    }
    printf("\n");
    print_cell("ID", 4); print_cell("Дата", 10); print_cell("Дисциплина", 22);
    print_cell("Группа", 10); print_cell("Преподаватель", 26); printf("\n");
    print_rule(76);
    for (int i = 0; i < n; i++) {
        print_cell_int(exams[i].exam_id, 4);
        print_cell(exams[i].exam_date, 10);
        print_cell(exams[i].subject_name, 22);
        print_cell(exams[i].group_name, 10);
        print_cell(exams[i].teacher_name, 26);
        printf("\n");
    }
}

void print_results(const ExamResult *results, int n, int columns)
{
    if (n == 0) {
        printf("Записей нет.\n");
        return;
    }

    int width = 0;
    printf("\n");
    if (columns & COL_STUDENT_ID) { print_cell("ID", 4); width += 5; }
    if (columns & COL_STUDENT)    { print_cell("Студент", 28); print_cell("Зачётка", 11); width += 41; }
    if (columns & COL_GROUP)      { print_cell("Группа", 9); width += 10; }
    if (columns & COL_EXAM)       { print_cell("Дисциплина", 20); print_cell("Дата", 10); width += 32; }
    if (columns & COL_TEACHER)    { print_cell("Преподаватель", 24); width += 25; }
    print_cell("Оценка", 24);
    width += 24;
    printf("\n");
    print_rule(width);

    for (int i = 0; i < n; i++) {
        const ExamResult *r = &results[i];
        if (columns & COL_STUDENT_ID) print_cell_int(r->student_id, 4);
        if (columns & COL_STUDENT)    { print_cell(r->student_name, 28); print_cell(r->record_book, 11); }
        if (columns & COL_GROUP)      print_cell(r->group_name, 9);
        if (columns & COL_EXAM)       { print_cell(r->subject_name, 20); print_cell(r->exam_date, 10); }
        if (columns & COL_TEACHER)    print_cell(r->teacher_name, 24);
        print_cell(grade_text(r->grade), 24);
        printf("\n");
    }
}
